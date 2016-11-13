#ifndef PAGE_H
#define PAGE_H

#include <QScopedPointer>

class PageProperties;
class QPdfWriter;

class Page
{
public:
    Page(QPdfWriter &pdfWriter);
    Page(const Page &otherPage);

    void addPageItem(PageItem *pPageItem);
    void clearPage();
    QList<PageItem*> pageItems() const;
    int bottomEdge() const;
    int leftEdge() const;
    int rightEdge() const;
    int pageHeight() const;
    int pageWidth() const;
    void setBottomMargin(int mm);
    void setLeftMargin(int mm);
    void setRightMargin(int mm);
    void setTopMargin(int mm);

    Page& operater=(const Page &otherPage);

private:
    QPdfWriter &m_pdfWriter;
    QScopedPointer<PageProperties> m_properties;
    QList<PageItem*> m_pageItems;
};

#endif
