#ifndef PAGE_H
#define PAGE_H

#include <QList>
#include <QScopedPointer>

class PageItem;
class PageProperties;
class QPdfWriter;

class Page
{
public:
    Page(QPdfWriter &pdfWriter);
    Page(const Page &other);
    ~Page();

    void addPageItem(PageItem *pPageItem);
    int bottomEdge() const;
    int bottomMargin() const;
    void clearPage();
    int leftEdge() const;
    int leftMargin() const;
    int pageHeight() const;
    QList<PageItem*> pageItems() const;
    int pageNumber() const;
    int pageWidth() const;
    int rightEdge() const;
    int rightMargin() const;
    void setBottomMargin(int mm);
    void setLeftMargin(int mm);
    void setPageNumber(int num);
    void setRightMargin(int mm);
    void setTopMargin(int mm);
    int topEdge() const;
    int topMargin() const;

    Page& operator=(const Page &other);

private:
    QPdfWriter &m_pdfWriter;
    QScopedPointer<PageProperties> m_properties;
    QList<PageItem*> m_pageItems;
};

#endif
