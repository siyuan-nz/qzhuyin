#include "pagebuilder.h"

#include "ast.h"
#include "movepageitem.h"
#include "page.h"
#include "pageitem.h"
#include "pageitemvisitor.h"

#include <cassert>
#include <functional>

#include <QFontMetrics>
#include <QPdfWriter>

const QHash<QChar, QChar> g_cZhNumHash = {
    { '0', 0x3007 },
    { '1', 0x4E00 },
    { '2', 0x4E8C },
    { '3', 0x4E09 },
    { '4', 0x56DB },
    { '5', 0x4E94 },
    { '6', 0x516D },
    { '7', 0x4E03 },
    { '8', 0x516B },
    { '9', 0x4E5D }
};

// A helper class for managing the resume stack of PageBuilder
template <class T>
class Resume
{
public:
    Resume(PageBuilder &pageBuilder, std::function<T()> resumeFunc);
    ~Resume();

private:
    PageBuilder &m_pageBuilder;
    std::function<T()> m_resumeFunc;
    AstNodeRef **m_ppNodeRef;
};

template <class T>
Resume<T>::Resume(PageBuilder &pageBuilder, std::function<T()> resumeFunc)
    : m_pageBuilder(pageBuilder)
    , m_resumeFunc(resumeFunc)
{
    // Insert a place holder into the stack to ensure correct stack ordering,
    // will be updated by the destructor if resuming is required.
    m_pageBuilder.m_resumeRefs.push(nullptr);
    m_ppNodeRef = &m_pageBuilder.m_resumeRefs.top();
}

template <class T>
Resume<T>::~Resume()
{
    if (m_pageBuilder.m_visitStatus == PageBuilder::eVisitStatus::Success)
        m_pageBuilder.m_resumeRefs.pop();
    else
        *m_ppNodeRef = m_resumeFunc();
}

class AlignColumn : protected PageItemVisitor
{
public:
    AlignColumn(PageBuilder &pageBuilder);

protected:
    void visit(Box &) override;
    void visit(LabelText &) override;
    void visit(LineText &) override;

private:
    void align();

    enum class eAlignMode {
        Anchor,
        Item
    } m_alignMode;
    PageBuilder &m_pageBuilder;
    PageItem &m_anchorItem;
    int m_anchorItemWidth;
};

AlignColumn::AlignColumn(PageBuilder& pageBuilder)
    : m_alignMode(eAlignMode::Anchor)
    , m_pageBuilder(pageBuilder)
    , m_anchorItem(*pageBuilder.widestItem(m_pageBuilder.m_currentColumn))
{
    if (m_pageBuilder.m_currentColumn.isEmpty())
        return;

    m_anchorItem.welcome(*this);
    align();
}

void AlignColumn::align()
{
    m_alignMode = eAlignMode::Item;

    for (auto item : m_pageBuilder.m_currentColumn) {
        if (item != &m_anchorItem)
            item->welcome(*this);
    }
}

void AlignColumn::visit(Box &box)
{
    switch (m_alignMode)
    {
    case eAlignMode::Anchor:
        m_anchorItemWidth = m_anchorItem.m_rect.width();
        break;
    case eAlignMode::Item:
        {
            int newX = m_anchorItem.m_rect.x() + (m_anchorItemWidth - box.m_rect.width()) / 2;
            MovePageItem(box, newX);
        }
        break;
    }
}

void AlignColumn::visit(LabelText &labelText)
{
    int rectWidth = labelText.m_rect.width();

    switch (m_alignMode)
    {
    case eAlignMode::Anchor:
        m_anchorItemWidth = rectWidth;
        break;
    case eAlignMode::Item:
        {
            int newX = m_anchorItem.m_rect.x() + (m_anchorItemWidth - rectWidth) / 2;
            labelText.m_rect.moveLeft(newX);
        }
        break;
    }
}

void AlignColumn::visit(LineText &lineText)
{
    const QFont &font = lineText.m_font;
    QFontMetrics fontMetrics(font, &m_pageBuilder.m_pdfWriter);
    int textWidth = fontMetrics.width(lineText.m_text.first().zhChar());

    switch (m_alignMode)
    {
    case eAlignMode::Anchor:
        m_anchorItemWidth = textWidth;
        break;
    case eAlignMode::Item:
        {
            int newX = m_anchorItem.m_rect.x() + (m_anchorItemWidth - textWidth) / 2;
            lineText.m_rect.moveLeft(newX);
        }
        break;
    }
}

PageBuilder::PageBuilder(QPdfWriter &pdfWriter, AstNode &root)
    : m_pdfWriter(pdfWriter)
    , m_rootNode(root)
    , m_pWidestItem(nullptr)
    , m_pCurrentBox(nullptr)
{
    m_xPage.reset(new Page(pdfWriter));
    m_xPage->setPageNumber(1);

    QFont defaultFont;
    m_fontStack.push(QFont(defaultFont, &m_pdfWriter));
}

PageBuilder::~PageBuilder()
{
}

Page* PageBuilder::nextPage()
{
    if (m_xPage && m_resumeRefs.isEmpty())
        m_rootNode.welcome(*this);
    else {
        while (!m_resumeRefs.isEmpty()) {
            AstNodeRef *pRef = m_resumeRefs.top();
            pRef->welcome(*this);

            if (m_visitStatus == eVisitStatus::Success) {
                m_resumeRefs.pop();
                delete pRef;
            } else
                break;
        }
    }

    if (m_visitStatus == eVisitStatus::Success)
        AlignColumn(*this);

    Page *pPage = m_xPage.take();
    if (!m_resumeRefs.isEmpty()) {
        m_xPage.reset(new Page(*pPage));
        m_xPage->clearPage();
        m_xPage->setPageNumber(m_xPage->pageNumber() + 1);
    }
    return pPage;
}

void PageBuilder::visit(HSpace &astNode)
{
    QFont currentFont = m_fontStack.top();
    QFontMetrics fontMetrics(currentFont, &m_pdfWriter);
    Box *pBox = new Box;
    pBox->m_rect.setWidth(fontMetrics.width(QChar(0x2000)) * astNode.m_space);
    pBox->m_rect.setHeight(m_xPage->pageHeight());
    m_visitStatus = fitItem(pBox);

    if (m_visitStatus == eVisitStatus::Success)
        m_xPage->addPageItem(pBox);
    else
        delete pBox;
}

void PageBuilder::visit(VSpace &astNode)
{
    QFont currentFont = m_fontStack.top();
    QFontMetrics fontMetrics(currentFont, &m_pdfWriter);
    Box *pBox = new Box;
    pBox->m_rect.setWidth(fontMetrics.width(QChar(0x2000)));
    pBox->m_rect.setHeight(fontMetrics.lineSpacing() * astNode.m_space);
    m_visitStatus = fitItem(pBox);

    if (m_visitStatus == eVisitStatus::Success) {
        if (m_pCurrentBox == nullptr) {
            m_pCurrentBox = new Box;
            m_xPage->addPageItem(m_pCurrentBox);
        }

        m_pCurrentBox->m_enclosedItems.append(pBox);
    } else
        delete pBox;
}

void PageBuilder::visit(Label &astNode)
{
    QFont currentFont = m_fontStack.top();
    QFontMetrics fontMetrics(currentFont, &m_pdfWriter);
    QString numString = QString::number(astNode.m_id);
    int textLength = numString.length() + 2; // +2 to accomodate parenthese
    float pointSizeF = currentFont.pointSize() / textLength;

    QString labelStr = QStringLiteral("︵");
    for (int i = 0; i < numString.length(); i++)
        labelStr += g_cZhNumHash.value(numString.at(i), QChar(0xFF1F));
    labelStr += QStringLiteral("︶");
    astNode.m_label = labelStr;

    // Get the width of the main text, otherwise the label might fit the
    // column but not the main text that follows.
    int lineWidth = fontMetrics.width(labelStr.at(0));
    LabelText *pLabelText = new LabelText;
    // A label should always take up the area of a single character
    pLabelText->m_rect.setWidth(lineWidth);
    pLabelText->m_rect.setHeight(fontMetrics.lineSpacing());
    pLabelText->m_text = labelStr;
    pLabelText->m_font = currentFont;
    pLabelText->m_font.setPointSizeF(pointSizeF);
    m_visitStatus = fitItem(pLabelText);

    if (m_visitStatus == eVisitStatus::Success) {
        if (m_pCurrentBox == nullptr) {
            m_pCurrentBox = new Box;
            m_xPage->addPageItem(m_pCurrentBox);
        }

        m_pCurrentBox->m_enclosedItems.append(pLabelText);
    } else
        delete pLabelText;
}

void PageBuilder::visit(Ref &astNode)
{
    QFont currentFont = m_fontStack.top();
    QFontMetrics fontMetrics(currentFont, &m_pdfWriter);
    LabelText *pLabelText = new LabelText;

    if (astNode.m_labelNode == nullptr)
        qFatal("PageBuilder::visit(Ref&): no link to its corresponding label");

    QString labelStr = astNode.m_labelNode->m_label;
    // Get the width of the main text, otherwise the ref might fit the column
    // but not the main text that follows.
    int lineWidth = fontMetrics.width(labelStr.at(0));
    float pointSizeF = currentFont.pointSize() / labelStr.length();
    // A label should always take up the area of a single character
    pLabelText->m_rect.setWidth(lineWidth);
    pLabelText->m_rect.setHeight(fontMetrics.lineSpacing());
    pLabelText->m_text = labelStr;
    pLabelText->m_font = currentFont;
    pLabelText->m_font.setPointSizeF(pointSizeF);
    m_visitStatus = fitItem(pLabelText);

    if (m_visitStatus == eVisitStatus::Success) {
        if (m_pCurrentBox == nullptr) {
            m_pCurrentBox = new Box;
            m_xPage->addPageItem(m_pCurrentBox);
        }

        m_pCurrentBox->m_enclosedItems.append(pLabelText);
    } else
        delete pLabelText;
}

void PageBuilder::visit(NewLine &)
{
    m_pWidestItem = widestItem(m_currentColumn);

    if (m_pWidestItem) {
        AlignColumn(*this);
        m_currentColumn.clear();
        m_visitStatus = eVisitStatus::Success;
    } else {
        HSpace hSpace;
        hSpace.m_space = 1;
        visit(hSpace);
    }
}

void PageBuilder::visit(NewPage &)
{
    endPage();
    m_visitStatus = eVisitStatus::EndPage;
}

void PageBuilder::visit(NewParagraph &astNode)
{
#if 0
    // TODO: Remove paragraph ref?
    Resume<NewParagraphRef *> resume(*this,
                                     [&astNode]() -> NewParagraphRef* {
        return new NewParagraphRef(astNode);
    });
#endif
    newParagraph();
}

void PageBuilder::visit(Scope &astNode)
{
    int position = 0;
    Resume<ScopeRef *> resume(*this,
                              [&position, &astNode]() -> ScopeRef* {
        return new ScopeRef(astNode, position);
    });

    m_fontStack.push(QFont(m_fontStack.top(), &m_pdfWriter));

    for (int i = 0; i < astNode.m_childNodes.count(); i++) {
        position = i;
        astNode.m_childNodes.at(i)->welcome(*this);
        if (m_visitStatus != eVisitStatus::Success) {
            break;
        }
    }

    switch (m_visitStatus) {
    case eVisitStatus::Success:
        m_fontStack.pop();
        break;
    case eVisitStatus::EndPage:
        position++;
        // Fall through
    case eVisitStatus::PageFull:
        endPage();
        break;
    }
}

void PageBuilder::visit(SetBottomMargin &astNode)
{
    m_xPage->setBottomMargin(astNode.m_mm);
    m_visitStatus = eVisitStatus::Success;
}

void PageBuilder::visit(SetFont &astNode)
{
    m_fontStack.top().setFamily(astNode.m_family);
    m_visitStatus = eVisitStatus::Success;
}

void PageBuilder::visit(SetFontSize &astNode)
{
    m_fontStack.top().setPointSize(astNode.m_pointSize);
    m_visitStatus = eVisitStatus::Success;
}

void PageBuilder::visit(SetLeftMargin &astNode)
{
    m_xPage->setLeftMargin(astNode.m_mm);
    m_visitStatus = eVisitStatus::Success;
}

void PageBuilder::visit(SetRightMargin &astNode)
{
    m_xPage->setRightMargin(astNode.m_mm);
    m_visitStatus = eVisitStatus::Success;
}

void PageBuilder::visit(SetTopMargin &astNode)
{
    m_xPage->setTopMargin(astNode.m_mm);
    m_visitStatus = eVisitStatus::Success;
}

void PageBuilder::visit(Text &astNode)
{
    int position = layoutText(astNode.m_text, 0);
    Resume<TextRef *> resume(*this,
                             [&position, &astNode]() -> TextRef* {
        return new TextRef(astNode, position);
    });

    if (m_visitStatus != eVisitStatus::Success)
        endPage();
}

void PageBuilder::visit(NewParagraphRef &)
{
    newParagraph();
}

void PageBuilder::visit(ScopeRef &astNode)
{
    Scope &scope = static_cast<Scope &>(astNode.m_astNode);
    int &position = astNode.m_position;

    // Revisiting nodes in the stack means we have to check if the previous
    // was successfully visited and move on to the next if so
    if (m_visitStatus == eVisitStatus::Success)
        position++;

    for (int i = position; i < scope.m_childNodes.count(); i++) {
        position = i;
        scope.m_childNodes.at(i)->welcome(*this);
        if (m_visitStatus != eVisitStatus::Success) {
            break;
        }
    }

    switch (m_visitStatus) {
    case eVisitStatus::Success:
        m_fontStack.pop();
        break;
    case eVisitStatus::EndPage:
        position++;
        // Fall through
    case eVisitStatus::PageFull:
        {
            endPage();
        }
        break;
    }
}

void PageBuilder::visit(TextRef &astNode)
{
    Text &text = static_cast<Text &>(astNode.m_astNode);
    astNode.m_position = layoutText(text.m_text, astNode.m_position);

    if (m_visitStatus != eVisitStatus::Success) {
        endPage();
    }
}

void PageBuilder::newParagraph()
{
    if (m_pCurrentBox) {
        m_pCurrentBox = nullptr;

        if (!m_currentColumn.isEmpty()) {
            NewLine newLine;
            visit(newLine);
        }
    }

    VSpace vSpace;
    vSpace.m_space = 2;
    visit(vSpace);
}

void PageBuilder::endPage()
{
    AlignColumn(*this);

    if (m_pCurrentBox) {
        if (m_pCurrentBox->m_enclosedItems.isEmpty())
            delete m_pCurrentBox;

        m_pCurrentBox = nullptr;
    }

    m_pWidestItem = nullptr;
    m_currentColumn.clear();
}

PageBuilder::eVisitStatus PageBuilder::fitItem(PageItem *pPageItem)
{
    int x;
    int y;
    QRect &itemRect = pPageItem->m_rect;

    if (m_xPage->pageItems().isEmpty()) {
        x = m_xPage->rightEdge() - pPageItem->m_rect.width();
        y = 0;
    } else if (m_currentColumn.isEmpty()) {
        if (m_pWidestItem)
            x = m_pWidestItem->m_rect.x() - pPageItem->m_rect.width();
        else
            x = m_xPage->rightEdge() - pPageItem->m_rect.width();
        y = 0;
    } else {
        PageItem *pLastPageItem = m_currentColumn.last();
        QRect &lastItemRect = pLastPageItem->m_rect;
        QRect widestItemRect = m_pWidestItem ? m_pWidestItem->m_rect : QRect();

        x = m_pWidestItem ? widestItemRect.x() - itemRect.width() : m_xPage->rightEdge() - itemRect.width();
        y = lastItemRect.y() + lastItemRect.height();

        if (y + itemRect.height() > m_xPage->bottomEdge()) {
            AlignColumn(*this);
            m_pWidestItem = widestItem(m_currentColumn);
            m_currentColumn.clear();
            x = m_pWidestItem->m_rect.x() - itemRect.width();
            y = 0;
        }
    }

    if (x < m_xPage->leftEdge())
        return eVisitStatus::PageFull;

    itemRect.moveTo(x, y);
    m_currentColumn.append(pPageItem);
    return eVisitStatus::Success;
}

PageItem* PageBuilder::widestItem(const QList<PageItem *> &items)
{
    PageItem *pWidestItem = nullptr;
    int widest = 0;

    for (auto item : items) {
        int itemWidth = item->m_rect.width();
        if (itemWidth > widest) {
            widest = itemWidth;
            pWidestItem = item;
        }
    }

    return pWidestItem;
}

int PageBuilder::layoutText(const QList<ZhChar> &text, int offset)
{
    QFont currentFont = m_fontStack.top();
    QFontMetrics fontMetrics(currentFont, &m_pdfWriter);
    int lineWidth = fontMetrics.width(text.first().zhChar()) * 5 / 3;
    int textLength = text.count() - offset;
    int columnHeight = m_currentColumn.isEmpty() ? 0
                                                 : m_currentColumn.last()->m_rect.y() +
                                                   m_currentColumn.last()->m_rect.height();
    int charsFirstLine = (m_xPage->pageHeight() - columnHeight) / fontMetrics.lineSpacing();

    if (m_pCurrentBox == nullptr) {
        m_pCurrentBox = new Box;
        m_xPage->addPageItem(m_pCurrentBox);
    }

    if (charsFirstLine) {
        if (textLength <= charsFirstLine)
            charsFirstLine = textLength;

        LineText *pLineText = new LineText;
        pLineText->m_rect.setWidth(lineWidth);
        pLineText->m_rect.setHeight(charsFirstLine * fontMetrics.lineSpacing());
        m_visitStatus = fitItem(pLineText);

        if (m_visitStatus == eVisitStatus::Success) {
            m_pCurrentBox->m_enclosedItems.append(pLineText);
            pLineText->m_text = text.mid(offset, charsFirstLine);
            pLineText->m_font = currentFont;

            if (charsFirstLine == textLength)
                return offset + textLength;
        } else {
            delete pLineText;
            return 0;
        }
    }

    int charsPerLine = m_xPage->pageHeight() / fontMetrics.lineSpacing();
    int fullLines = (textLength - charsFirstLine) / charsPerLine;
    int charsLastLine = (textLength - charsFirstLine) % charsPerLine;

    for (int i = 0; i < fullLines; i++) {
        int processedChars = offset + charsFirstLine + i * charsPerLine;
        LineText *pLineText = new LineText;
        pLineText->m_rect.setWidth(lineWidth);
        pLineText->m_rect.setHeight(m_xPage->pageHeight());
        m_visitStatus = fitItem(pLineText);

        if (m_visitStatus == eVisitStatus::Success) {
            m_pCurrentBox->m_enclosedItems.append(pLineText);
            pLineText->m_text = text.mid(processedChars, charsPerLine);
            pLineText->m_font = currentFont;
        } else {
            delete pLineText;
            return processedChars;
        }
    }

    if (charsLastLine) {
        int processedChars = offset + textLength - charsLastLine;
        LineText *pLineText = new LineText;
        pLineText->m_rect.setWidth(lineWidth);
        pLineText->m_rect.setHeight(charsLastLine * fontMetrics.lineSpacing());
        m_visitStatus = fitItem(pLineText);

        if (m_visitStatus == eVisitStatus::Success) {
            m_pCurrentBox->m_enclosedItems.append(pLineText);
            pLineText->m_text = text.mid(processedChars);
            pLineText->m_font = currentFont;
        } else {
            delete pLineText;
            return processedChars;
        }
    }

    return text.count();
}
