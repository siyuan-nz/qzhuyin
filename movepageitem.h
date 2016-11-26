#ifndef MOVEPAGEITEM_H
#define MOVEPAGEITEM_H

#include "pageitemvisitor.h"

class PageItem;

class MovePageItem : protected PageItemVisitor
{
public:
    MovePageItem(PageItem &pageItem, int x);

protected:
    void visit(Box &) override;
    void visit(LineText &) override;

private:
    void translateItem(PageItem &pageItem);
    int m_translateX;
};

#endif
