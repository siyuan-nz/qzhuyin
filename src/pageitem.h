#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QFont>
#include <QRect>

#include "zhchar.h"

class PageItemVisitor;

class PageItem
{
public:
    virtual ~PageItem();
    virtual QString className() const = 0;
    virtual void welcome(PageItemVisitor &visitor) = 0;

    QRect m_rect;
};

class Box : public PageItem
{
public:
    ~Box();
    QString className() const override;
    void welcome(PageItemVisitor &visitor) override;

    QList<PageItem *> m_enclosedItems;
};

class LabelText : public PageItem
{
public:
    QString className() const override;
    void welcome(PageItemVisitor &visitor) override;

    QString m_text;
    QFont m_font;
};

class LineText : public PageItem
{
public:
    QString className() const override;
    void welcome(PageItemVisitor &visitor) override;

    QList<ZhChar> m_text;
    QFont m_font;
};

#endif
