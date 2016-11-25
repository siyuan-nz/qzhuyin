#include "pagedebugoutput.h"
#include "page.h"
#include "pageitem.h"

#include <iostream>

#include <QDebug>

PageDebugOutput::PageDebugOutput(Page &page)
{
    qDebug("Page %d\n"
           "    Top margin: %dmm\n"
           "    Bottom margin: %dmm\n"
           "    Left margin: %dmm\n"
           "    Right margin: %dmm\n"
           "    Page viewport: %dx%d\n",
           page.pageNumber(),
           page.topMargin(),
           page.bottomMargin(),
           page.leftMargin(),
           page.rightMargin(),
           page.pageWidth(),
           page.pageHeight());

    m_indentLevel = 1;
    for (auto item : page.pageItems())
        item->welcome(*this);
}

QString PageDebugOutput::indent() const
{
    QString spaces;

    for (int i = 0; i < m_indentLevel; i++)
        spaces += "    ";

    return spaces;
}

void PageDebugOutput::visit(Box &box)
{
    QString debugMsg;
    QDebug debug(&debugMsg);
    debug << box.className() << box.m_rect;
    qDebug("%s%s", qPrintable(indent()), qPrintable(debugMsg));

    m_indentLevel++;
    for (auto item : box.m_enclosedItems) {
        item->welcome(*this);
    }
    m_indentLevel--;
}

void PageDebugOutput::visit(LineText &lineText)
{
    QString debugMsg;
    QString text;
    QDebug debug(&debugMsg);

    for (const ZhChar &zhChar : lineText.m_text)
        text += zhChar.zhChar();

    debug << lineText.className() << lineText.m_rect << ':' << text;
    qDebug("%s%s", qPrintable(indent()), qPrintable(debugMsg));
}
