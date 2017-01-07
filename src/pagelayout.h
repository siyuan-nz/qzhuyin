#ifndef PAGELAYOUT_H
#define PAGELAYOUT_H

#include "pageitemvisitor.h"

#include <QRect>
#include <QList>

class Box;
class Page;
class PageItem;

class PageLayout : protected PageItemVisitor
{
public:
    QString className() const override;
    void layout(Page &page);

protected:
    void visit(Box &) override;

private:
    void adjustGaps(Page &page, const QList<Box *> &boxes);

    QRect m_boxBoundingRect;
    QList<Box *> m_boxes;
};

#endif
