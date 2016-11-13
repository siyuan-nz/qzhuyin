#ifndef PAGEITEMVISITOR_H
#define PAGEITEMVISITOR_H

class PageItem;
class Box;
class LineText;

class PageItemVisitor
{
public:
    void visit(PageItem &) { qWarning("visit(PageItem &) unimplemented"); }
    void visit(Box &) { qWarning("visit(Box &) unimplemented"); }
    void visit(LineText &) { qWarning("visit(LineText &) unimplemented"); }
};

#endif
