#include "page.h"

#include <QPdfWriter>

class PageProperties
{
public:
    PageProperties();

    QMarginsF m_margin;
    QPageLayout m_pageLayout;
    int m_pageNumber;
};

// TODO: Support other page sizes
PageProperties::PageProperties()
    : m_margin(20, 20, 20, 20)
    , m_pageLayout(QPageSize(QPageSize::A4),
                   QPageLayout::Portrait,
                   m_margin,
                   QPageLayout::Millimeter)
    , m_pageNumber(1)
{
}

Page::Page(QPdfWriter &pdfWriter)
    : m_pdfWriter(pdfWriter)
{
    m_properties.reset(new PageProperties);
    m_pdfWriter.setPageLayout(m_properties->m_pageLayout);
}

Page::Page(const Page &other)
    : Page(other.m_pdfWriter)
{
    *m_properties = *other.m_properties;
    m_pageItems = other.m_pageItems;
}

Page::~Page()
{
}

void Page::addPageItem(PageItem *pPageItem)
{
    m_pageItems.append(pPageItem);
}

void Page::clearPage()
{
    m_pageItems.clear();
}

QList<PageItem*> Page::pageItems() const
{
    return m_pageItems;
}

int Page::bottomEdge() const
{
    return pageHeight();
}

int Page::leftEdge() const
{
    return 0;
}

int Page::rightEdge() const
{
    return pageWidth();
}

int Page::topEdge() const
{
    return 0;
}

int Page::pageHeight() const
{
    return m_pdfWriter.height();
}

int Page::pageWidth() const
{
    return m_pdfWriter.width();
}

int Page::bottomMargin() const
{
    return m_properties->m_margin.bottom();
}

int Page::leftMargin() const
{
    return m_properties->m_margin.left();
}

int Page::rightMargin() const
{
    return m_properties->m_margin.right();
}

int Page::topMargin() const
{
    return m_properties->m_margin.top();
}

void Page::setBottomMargin(int mm)
{
    m_properties->m_margin.setBottom(mm);
    m_pdfWriter.setPageMargins(m_properties->m_margin);
}

void Page::setLeftMargin(int mm)
{
    m_properties->m_margin.setLeft(mm);
    m_pdfWriter.setPageMargins(m_properties->m_margin);
}

void Page::setRightMargin(int mm)
{
    m_properties->m_margin.setRight(mm);
    m_pdfWriter.setPageMargins(m_properties->m_margin);
}

void Page::setTopMargin(int mm)
{
    m_properties->m_margin.setTop(mm);
    m_pdfWriter.setPageMargins(m_properties->m_margin);
}

int Page::pageNumber() const
{
    return m_properties->m_pageNumber;
}

void Page::setPageNumber(int num)
{
    m_properties->m_pageNumber = num;
}
