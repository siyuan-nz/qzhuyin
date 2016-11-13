#ifndef PAGEBUILDER_H
#define PAGEBUILDER_H

#include "astvisitor.h"
#include "page.h"

#include <QStack>

class AstNode;
class Box;
class PageItem;

class PageBuilder : public AstVisitor
{
public:
    PageBuilder(QPdfWriter &pdfWriter, AstNode &root);

    Page* NextPage();

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
    void visit(ScopeRef &) override;
    void visit(TextRef &) override;

private:
    void endPage(AstNodeRef *pAstNodeRef);
    bool fitItem(PageItem *pPageItem);
    PageItem* widestItem(const QList<PageItem *> &items);
    int layoutText(const QList<ZhChar> &text, int offset);

    QPdfWriter &m_pdfWriter;
    AstNode &m_rootNode;
    QStack<QFont> m_fontStack;
    QStack<AstNodeRef *> m_resumeRefs;
    QScopedPointer<Page> m_xPage;
    PageItem *m_pWidestItem;
    QList<PageItem *> m_currentColumn;
    Box *m_pCurrentBox;

    enum class eVisitStatus {
        Success,
        PageFull,
        EndPage
    } m_visitStatus;
};

#endif
