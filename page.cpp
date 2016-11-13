#include "page.h"

class PageProperties
{
public:
    int m_bottomMarginMm;
    int m_leftMarginMm;
    int m_rightMarginMm;
    int m_topMarginMm;
};

Page::Page(QPdfWriter &pdfWriter)
    : m_pdfWriter(pdfWriter)
{
    m_properties.reset(new PageProperties);
}
