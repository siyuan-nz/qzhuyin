#include "pagelayout.h"
#include "page.h"
#include "pageitem.h"

#include <QRegion>

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

void PageLayout::layout(Page &page)
{
    QList<PageItem *> pageItems = page.pageItems();

    m_boxes.clear();

    for (auto item : pageItems)
        item->welcome(*this);

    adjustGaps(page, m_boxes);
}

void PageLayout::visit(Box &box)
{
    QRegion boxRegion;

    // Must have a valid height to be taken account into the calculation
    if (box.m_rect.height() == 0)
        box.m_rect.setHeight(1);

    boxRegion |= box.m_rect;

    for (auto item : box.m_enclosedItems) {

        boxRegion |= item->m_rect;
    }

    m_boxBoundingRect = boxRegion.boundingRect();

    if (!m_boxBoundingRect.isEmpty()) {
        box.m_rect = m_boxBoundingRect;
        m_boxes.append(&box);
    }
}

void PageLayout::adjustGaps(Page &page, const QList<Box *> &boxes)
{
    int maxGap = page.pageWidth() / 10;
    int occupiedWidth = 0;

    for (auto box : boxes)
        occupiedWidth += box->m_rect.width();

    int remainingWidth = page.pageWidth() - occupiedWidth;

    if (boxes.count() > 1) {
        int gap = remainingWidth / (boxes.count() - 1);

        if (gap <= maxGap) {
            Box *pPrevBox = nullptr;

            for (auto box : boxes) {
                int x;

                if (pPrevBox)
                    x = pPrevBox->m_rect.x() - gap - box->m_rect.width();
                else
                    x = page.pageWidth() - box->m_rect.width();

                MovePageItem movePageItem(*box, x);
                pPrevBox = box;
            }
        }
    }
}
