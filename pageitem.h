#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QFont>
#include <QRect>

#include "zhchar.h"

class PageItemVisitor;

class PageItem
{
public:
    virtual void welcome(PageItemVisitor &visitor) = 0;

    QRect m_rect;
};

class Box : public PageItem
{
public:
    void welcome(PageItemVisitor &visitor) override;

    QList<PageItem *> m_enclosedItems;
};

class LineText : public PageItem
{
public:
    void welcome(PageItemVisitor &visitor) override;

    QList<ZhChar> m_text;
    QFont m_font;
};

#endif
