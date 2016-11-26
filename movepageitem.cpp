#include "movepageitem.h"
#include "pageitem.h"

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

void MovePageItem::visit(LineText &lineText)
{
    translateItem(lineText);
}
