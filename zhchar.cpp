#include "zhchar.h"

#include <QPainter>

const QString ZhChar::m_Tones = "ˊˇˋ˙";

ZhChar::ZhChar(const QChar &zhChar, const QString &zhuYin)
    : m_Char(zhChar)
{
    m_ZhuYin = zhuYin;
    if (m_Tones.contains(zhuYin.at(zhuYin.length() - 1))) {
        m_Tone = zhuYin.at(zhuYin.length() - 1);
        m_ZhuYin.chop(1);
    }
}

void ZhChar::draw(QPainter &painter, int x, int y)
{
    if (m_Char.isNull())
        return;

    const QFontMetrics &fontMetrics = painter.fontMetrics();

    // Move y one character down as y corresponds to the bottom left corner of the character
    int charY = y + fontMetrics.height();
    painter.drawText(x, charY, m_Char);
    x += fontMetrics.width(m_Char);

    if (m_ZhuYin.isEmpty())
        return;

    painter.save();

    QFont zhuYinFont = painter.font();
    zhuYinFont.setPointSize(painter.font().pointSize() / 3);
    painter.setFont(zhuYinFont);

    const QFontMetrics &zhuYinFontMetrics = painter.fontMetrics();
    const int zhCharFontHeight = fontMetrics.lineSpacing();
    const int zhuYinFontHeight = zhuYinFontMetrics.lineSpacing();
    const int zhuYinY = y + zhuYinFontHeight;
    // Vertically align zhu yin
    int yOffset = zhuYinY + (zhCharFontHeight - zhuYinFontHeight * m_ZhuYin.length()) / 2;
    ZhChar::drawText(painter, x, yOffset, m_ZhuYin);

    if (m_Tone.isNull()) {
        painter.restore();
        return;
    }

    x += zhuYinFontMetrics.width(m_Tone);
    // Align the tone to half way between the last 2 zhu yin characters
    yOffset = yOffset + zhuYinFontHeight / 2 + zhuYinFontHeight * (m_ZhuYin.length() - 2);
    painter.drawText(x, yOffset, m_Tone);

    painter.restore();
}

void ZhChar::drawText(QPainter &painter, int x, int y, const QString &text)
{
    for (auto c : text)
    {
        painter.drawText(x, y, c);
        y += painter.fontMetrics().lineSpacing();
    }
}
