#ifndef PAGEITEMVISITOR_H
#define PAGEITEMVISITOR_H

#include <QtGlobal>

class Box;
class LabelText;
class LineText;

class PageItemVisitor
{
public:
    virtual void visit(Box &) { qWarning("visit(Box &) unimplemented"); }
    virtual void visit(LabelText &) { qWarning("visit(LabelText &) unimplemented"); }
    virtual void visit(LineText &) { qWarning("visit(LineText &) unimplemented"); }
};

#endif
