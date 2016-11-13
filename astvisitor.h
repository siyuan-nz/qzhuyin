#ifndef ASTVISITOR_H
#define ASTVISITOR_H

class HSpace;
class VSpace;
class NewLine;
class NewPage;
class NewParagraph;
class Scope;
class SetFont;
class SetFontSize;
class SetBottomMargin;
class SetLeftMargin;
class SetRightMargin;
class SetTopMargin;
class Text;
class ScopeRef;
class TextRef;

class AstVisitor
{
public:
    virtual void visit(HSpace &) { qWarning("visit(HSpace &) unimplemented"); }
    virtual void visit(VSpace &) { qWarning("visit(VSpace &) unimplemented"); }
    virtual void visit(NewLine &) { qWarning("visit(NewLine &) unimplemented"); }
    virtual void visit(NewPage &) { qWarning("visit(NewPage &) unimplemented"); }
    virtual void visit(NewParagraph &) { qWarning("visit(NewParagraph &) unimplemented"); }
    virtual void visit(Scope &) { qWarning("visit(Scope &) unimplemented"); }
    virtual void visit(SetFont &) { qWarning("visit(SetFont &) unimplemented"); }
    virtual void visit(SetFontSize &) { qWarning("visit(SetFontSize &) unimplemented"); }
    virtual void visit(SetBottomMargin &) { qWarning("visit(SetBottomMargin &) unimplemented"); }
    virtual void visit(SetLeftMargin &) { qWarning("visit(SetLeftMargin &) unimplemented"); }
    virtual void visit(SetRightMargin &) { qWarning("visit(SetRightMargin &) unimplemented"); }
    virtual void visit(SetTopMargin &) { qWarning("visit(SetTopMargin &) unimplemented"); }
    virtual void visit(Text &) { qWarning("visit(Text &) unimplemented"); }
    virtual void visit(ScopeRef &) { qWarning("visit(ScopeRef &) unimplemented"); }
    virtual void visit(TextRef &) { qWarning("visit(TextRef &) unimplemented"); }
};

#endif
