#ifndef PAGEBUILDER_H
#define PAGEBUILDER_H

#include "astvisitor.h"
#include "zhchar.h"

#include <QFont>
#include <QStack>

class AstNode;
class AstNodeRef;
class Box;
class Page;
class PageItem;
class QPdfWriter;

class PageBuilder : protected AstVisitor
{
public:
    PageBuilder(QPdfWriter &pdfWriter, AstNode &root);
    ~PageBuilder();

    Page* nextPage();

protected:
    void visit(HSpace &) override;
    void visit(VSpace &) override;
    void visit(NewLine &) override;
    void visit(NewPage &) override;
    void visit(NewParagraph &) override;
    void visit(Scope &) override;
    void visit(SetFont &) override;
    void visit(SetFontSize &) override;
    void visit(SetBottomMargin &) override;
    void visit(SetLeftMargin &) override;
    void visit(SetRightMargin &) override;
    void visit(SetTopMargin &) override;
    void visit(Text &) override;
    void visit(NewParagraphRef &) override;
    void visit(ScopeRef &) override;
    void visit(TextRef &) override;

private:
    enum class eVisitStatus {
        Success,
        PageFull,
        EndPage
    };

    void newParagraph();
    void endPage();
    eVisitStatus fitItem(PageItem *pPageItem);
    PageItem* widestItem(const QList<PageItem *> &items);
    int layoutText(const QList<ZhChar> &text, int offset);
    void layoutPage(Page &page);

    QPdfWriter &m_pdfWriter;
    AstNode &m_rootNode;
    QStack<QFont> m_fontStack;
    QStack<AstNodeRef *> m_resumeRefs;
    QScopedPointer<Page> m_xPage;
    PageItem *m_pWidestItem;
    QList<PageItem *> m_currentColumn;
    Box *m_pCurrentBox;
    eVisitStatus m_visitStatus;

    template <class T>
    friend class Resume;
};

#endif
