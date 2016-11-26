#include "pagelayout.h"
#include "movepageitem.h"
#include "page.h"
#include "pageitem.h"

#include <QRegion>

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

                MovePageItem(*box, x);
                pPrevBox = box;
            }
        }
    }
}
