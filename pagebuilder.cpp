#include "pagebuilder.h"

#include "ast.h"
#include "page.h"
#include "pageitem.h"

#include <cassert>

#include <QFontMetrics>
#include <QPdfWriter>

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
    pBox->m_rect.setWidth(0);
    pBox->m_rect.setHeight(fontMetrics.lineSpacing() * astNode.m_space);
    m_visitStatus = fitItem(pBox);

    if (m_visitStatus == eVisitStatus::Success)
        m_xPage->addPageItem(pBox);
    else
        delete pBox;
}

void PageBuilder::visit(NewLine &)
{
    m_pWidestItem = widestItem(m_currentColumn);

    if (m_pWidestItem) {
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
    endPage(nullptr);
    m_visitStatus = eVisitStatus::EndPage;
}

void PageBuilder::visit(NewParagraph &)
{
    if (m_pCurrentBox) {
        NewLine newLine;
        visit(newLine);
    }

    VSpace vSpace;
    vSpace.m_space = 2;
    visit(vSpace);

    if (m_visitStatus == eVisitStatus::Success) {
        m_pCurrentBox = new Box;
        m_xPage->addPageItem(m_pCurrentBox);
    }
}

void PageBuilder::visit(Scope &astNode)
{
    int position = 0;

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
        {
            ScopeRef *pScopeRef = new ScopeRef(astNode, position);
            endPage(pScopeRef);
        }
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

    if (m_visitStatus != eVisitStatus::Success) {
        TextRef *pTextRef = new TextRef(astNode, position);
        endPage(pTextRef);
    }
}

void PageBuilder::visit(ScopeRef &astNode)
{
    Scope &scope = static_cast<Scope &>(astNode.m_astNode);
    int &position = astNode.m_position;

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
            endPage(nullptr);
        }
        break;
    }
}

void PageBuilder::visit(TextRef &astNode)
{
    Text &text = static_cast<Text &>(astNode.m_astNode);
    astNode.m_position = layoutText(text.m_text, astNode.m_position);

    if (m_visitStatus != eVisitStatus::Success) {
        endPage(nullptr);
    }
}

void PageBuilder::endPage(AstNodeRef *pAstNodeRef)
{
    if (pAstNodeRef)
        m_resumeRefs.push(pAstNodeRef);

    m_pWidestItem = nullptr;
    m_pCurrentBox = nullptr;
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
        assert(m_pWidestItem);
        x = m_pWidestItem->m_rect.x() - pPageItem->m_rect.width();
        y = 0;
    } else {
        PageItem *pLastPageItem = m_currentColumn.last();
        QRect &lastItemRect = pLastPageItem->m_rect;
        QRect widestItemRect = m_pWidestItem ? m_pWidestItem->m_rect : QRect();

        x = m_pWidestItem ? widestItemRect.x() - itemRect.width() : m_xPage->rightEdge() - itemRect.width();
        y = lastItemRect.y() + lastItemRect.height();

        if (y + itemRect.height() > m_xPage->bottomEdge()) {
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

    if (charsFirstLine) {
        if (textLength <= charsFirstLine)
            charsFirstLine = textLength;

        LineText *pLineText = new LineText;
        pLineText->m_rect.setWidth(lineWidth);
        pLineText->m_rect.setHeight(charsFirstLine * fontMetrics.lineSpacing());
        pLineText->m_text = text.mid(offset, charsFirstLine);
        pLineText->m_font = currentFont;
        m_pCurrentBox->m_enclosedItems.append(pLineText);
        m_visitStatus = fitItem(pLineText);

        if (charsFirstLine == textLength)
            return textLength;
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
