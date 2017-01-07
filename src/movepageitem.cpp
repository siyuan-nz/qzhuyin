#include "movepageitem.h"
#include "common.h"
#include "pageitem.h"

CLASSNAME(MovePageItem)

MovePageItem::MovePageItem(PageItem& pageItem, int x)
    : m_translateX(x - pageItem.m_rect.x())
{
    if (m_translateX)
        pageItem.welcome(*this);
}

void MovePageItem::translateItem(PageItem &pageItem)
{
    pageItem.m_rect.translate(m_translateX, 0);
}

void MovePageItem::visit(Box &box)
{
    translateItem(box);
    for (auto item : box.m_enclosedItems)
        item->welcome(*this);
}

void MovePageItem::visit(EllipsisText &ellipsisText)
{
    translateItem(ellipsisText);
}

void MovePageItem::visit(LabelText &labelText)
{
    translateItem(labelText);
}

void MovePageItem::visit(LineText &lineText)
{
    translateItem(lineText);
}
