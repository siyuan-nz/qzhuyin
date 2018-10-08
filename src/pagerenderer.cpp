#include "pagerenderer.h"
#include "common.h"
#include "page.h"
#include "pageitem.h"
#include "zhchar.h"

#include <QPdfWriter>
#include <QString>

CLASSNAME(PageRenderer)

PageRenderer::PageRenderer(QPdfWriter& pdfWriter)
    : m_pdfWriter(pdfWriter)
    , m_painter(&pdfWriter)
{
}

void PageRenderer::render(Page &page)
{
    for (auto item : page.pageItems())
        item->welcome(*this);
}

void PageRenderer::visit(Box &box)
{
    for (auto item : box.m_enclosedItems)
        item->welcome(*this);
}

void PageRenderer::visit(EllipsisText &ellipsisText)
{
    m_painter.setFont(ellipsisText.m_font);

    const QChar dot(0xFF0E);
    const QFontMetrics &fontMetrics = m_painter.fontMetrics();
    QRect dotRect = fontMetrics.boundingRect(dot);
    for (int i = 0; i < 3; i++) {
        int x = ellipsisText.m_rect.x() +
                (ellipsisText.m_rect.width() - fontMetrics.width(dot)) / 2;
        // -------  gap
        // ---o--- _/
        // -------
        // ---o--- centerAtY
        // -------
        // ---o---
        // -------
        int gap = ellipsisText.m_rect.height() / 6;
        int centerAtY = ellipsisText.m_rect.y() + gap / 2 + i * 2 * gap;
        int y = centerAtY - dotRect.y() + dotRect.height();
        drawText(x, y, QString(dot));
    }
}

void PageRenderer::visit(LabelText &labelText)
{
    m_painter.setFont(labelText.m_font);

    const QFontMetrics &fontMetrics = m_painter.fontMetrics();
    int x = labelText.m_rect.x() + (labelText.m_rect.width() - fontMetrics.width(labelText.m_text.at(0))) / 2;
    int y = labelText.m_rect.y() + fontMetrics.height();
    drawText(x, y, labelText.m_text);
}

void PageRenderer::visit(LineText &lineText)
{
    m_painter.setFont(lineText.m_font);

    const QFontMetrics &fontMetrics = m_painter.fontMetrics();
    int x = lineText.m_rect.x();
    int y = lineText.m_rect.y();

    for (const ZhChar &zhChar : lineText.m_text) {
        drawZhChar(x, y, zhChar);
        y += fontMetrics.lineSpacing();
    }
}

void PageRenderer::drawZhChar(int x, int y, const ZhChar &zhChar)
{
    const QFontMetrics &fontMetrics = m_painter.fontMetrics();

    // Move y one character down as y corresponds to the bottom left corner of the character
    int charY = y + fontMetrics.height();
    m_painter.drawText(x, charY, zhChar.zhChar());
    x += fontMetrics.width(zhChar.zhChar());

    QString zhuYin = zhChar.zhuYin();
    if (zhuYin.isEmpty())
        return;

    m_painter.save();

    QFont zhuYinFont = m_painter.font();
    zhuYinFont.setPointSize(m_painter.font().pointSize() / 4);
    m_painter.setFont(zhuYinFont);

    const QFontMetrics &zhuYinFontMetrics = m_painter.fontMetrics();
    const int zhCharFontHeight = fontMetrics.lineSpacing();
    const int zhuYinFontHeight = zhuYinFontMetrics.lineSpacing();
    const int zhuYinY = y + zhuYinFontHeight;
    // Vertically align zhu yin
    int yOffset = zhuYinY + (zhCharFontHeight - zhuYinFontHeight * zhuYin.length()) / 2;
    drawText(x, yOffset, zhuYin);

    if (zhChar.tone().isNull()) {
        m_painter.restore();
        return;
    }

    x += zhuYinFontMetrics.width(zhChar.tone()) * 2 / 3;
    // Align the tone to half way between the last 2 zhu yin characters
    yOffset = yOffset + zhuYinFontHeight / 2 + zhuYinFontHeight * (zhuYin.length() - 2);
    m_painter.drawText(x, yOffset, zhChar.tone());
    m_painter.restore();
}

void PageRenderer::drawText(int x, int y, const QString &text)
{
    for (auto const &c : text)
    {
        m_painter.drawText(x, y, c);
        y += m_painter.fontMetrics().lineSpacing();
    }
}
