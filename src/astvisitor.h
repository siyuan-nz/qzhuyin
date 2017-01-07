#ifndef ASTVISITOR_H
#define ASTVISITOR_H

#include <QtGlobal>
#include <QString>

class Ellipsis;
class HSpace;
class VSpace;
class Label;
class Ref;
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
class EllipsisRef;
class NewParagraphRef;
class ScopeRef;
class TextRef;

class AstVisitor
{
public:
    virtual QString className() const = 0;

    virtual void visit(Ellipsis &) { qWarning("%s::visit(Ellipsis &) unimplemented", qPrintable(className())); }
    virtual void visit(HSpace &) { qWarning("%s::visit(HSpace &) unimplemented", qPrintable(className())); }
    virtual void visit(VSpace &) { qWarning("%s::visit(VSpace &) unimplemented", qPrintable(className())); }
    virtual void visit(Label &) { qWarning("%s::visit(Label &) unimplemented", qPrintable(className())); }
    virtual void visit(Ref &) { qWarning("%s::visit(Ref &) unimplemented", qPrintable(className())); }
    virtual void visit(NewLine &) { qWarning("%s::visit(NewLine &) unimplemented", qPrintable(className())); }
    virtual void visit(NewPage &) { qWarning("%s::visit(NewPage &) unimplemented", qPrintable(className())); }
    virtual void visit(NewParagraph &) { qWarning("%s::visit(NewParagraph &) unimplemented", qPrintable(className())); }
    virtual void visit(Scope &) { qWarning("%s::visit(Scope &) unimplemented", qPrintable(className())); }
    virtual void visit(SetFont &) { qWarning("%s::visit(SetFont &) unimplemented", qPrintable(className())); }
    virtual void visit(SetFontSize &) { qWarning("%s::visit(SetFontSize &) unimplemented", qPrintable(className())); }
    virtual void visit(SetBottomMargin &) { qWarning("%s::visit(SetBottomMargin &) unimplemented", qPrintable(className())); }
    virtual void visit(SetLeftMargin &) { qWarning("%s::visit(SetLeftMargin &) unimplemented", qPrintable(className())); }
    virtual void visit(SetRightMargin &) { qWarning("%s::visit(SetRightMargin &) unimplemented", qPrintable(className())); }
    virtual void visit(SetTopMargin &) { qWarning("%s::visit(SetTopMargin &) unimplemented", qPrintable(className())); }
    virtual void visit(Text &) { qWarning("%s::visit(Text &) unimplemented", qPrintable(className())); }
    virtual void visit(EllipsisRef &) { qWarning("%s::visit(EllipsisRef &) unimplemented", qPrintable(className())); };
    virtual void visit(NewParagraphRef &) { qWarning("%s::visit(NewParagraphRef &) unimplemented", qPrintable(className())); }
    virtual void visit(ScopeRef &) { qWarning("%s::visit(ScopeRef &) unimplemented", qPrintable(className())); }
    virtual void visit(TextRef &) { qWarning("%s::visit(TextRef &) unimplemented", qPrintable(className())); }
};

#endif
