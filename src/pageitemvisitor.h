#ifndef PAGEITEMVISITOR_H
#define PAGEITEMVISITOR_H

#include <QtGlobal>
#include <QString>

class Box;
class EllipsisText;
class LabelText;
class LineText;

class PageItemVisitor
{
public:
    virtual QString className() const = 0;

    virtual void visit(Box &) { qWarning("%s::visit(Box &) unimplemented", qPrintable(className())); }
    virtual void visit(EllipsisText &) { qWarning("%s::visit(EllipsisText &) unimplemented", qPrintable(className())); }
    virtual void visit(LabelText &) { qWarning("%s::visit(LabelText &) unimplemented", qPrintable(className())); }
    virtual void visit(LineText &) { qWarning("%s::visit(LineText &) unimplemented", qPrintable(className())); }
};

#endif
