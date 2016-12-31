#ifndef ASTPRINTER_H
#define ASTPRINTER_H

#include "astvisitor.h"

class AstNode;
class QString;

class AstPrinter : public AstVisitor
{
public:
    AstPrinter(AstNode &root);

protected:
    void visit(HSpace &) override;
    void visit(VSpace &) override;
    void visit(Label &) override;
    void visit(Ref &) override;
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
    void printIndent();
    void printLine(const QString &message);

    int m_indentLevel = 0;
};

#endif
