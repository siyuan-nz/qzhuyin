#include "zhchar.h"

#include <QPainter>

const QString ZhChar::m_tones = "ˊˇˋ˙";

ZhChar::ZhChar(const QChar &zhChar, const QString &zhuYin)
    : m_char(zhChar)
{
    parseZhuYin(zhuYin);
}

ZhChar::ZhChar(const ZhChar &other)
    : m_char(other.m_char)
    , m_tone(other.m_tone)
    , m_zhuYin(other.m_zhuYin)
{
}

ZhChar::ZhChar(ZhChar &&other)
    : m_char(std::move(other.m_char))
    , m_tone(std::move(other.m_tone))
    , m_zhuYin(std::move(other.m_zhuYin))
{
}

ZhChar& ZhChar::operator=(const ZhChar &other)
{
    m_char = other.m_char;
    m_tone = other.m_tone;
    m_zhuYin = other.m_zhuYin;
    return *this;
}

ZhChar& ZhChar::operator=(ZhChar &&other)
{
    m_char = std::move(other.m_char);
    m_tone = std::move(other.m_tone);
    m_zhuYin = std::move(other.m_zhuYin);
    return *this;
}

void ZhChar::draw(QPainter &painter, int x, int y)
{
    if (m_char.isNull())
        return;

    const QFontMetrics &fontMetrics = painter.fontMetrics();

    // Move y one character down as y corresponds to the bottom left corner of the character
    int charY = y + fontMetrics.height();
    painter.drawText(x, charY, m_char);
    x += fontMetrics.width(m_char);

    if (m_zhuYin.isEmpty())
        return;

    painter.save();

    QFont zhuYinFont = painter.font();
    zhuYinFont.setPointSize(painter.font().pointSize() / 4);
    painter.setFont(zhuYinFont);

    const QFontMetrics &zhuYinFontMetrics = painter.fontMetrics();
    const int zhCharFontHeight = fontMetrics.lineSpacing();
    const int zhuYinFontHeight = zhuYinFontMetrics.lineSpacing();
    const int zhuYinY = y + zhuYinFontHeight;
    // Vertically align zhu yin
    int yOffset = zhuYinY + (zhCharFontHeight - zhuYinFontHeight * m_zhuYin.length()) / 2;
    ZhChar::drawText(painter, x, yOffset, m_zhuYin);

    if (m_tone.isNull()) {
        painter.restore();
        return;
    }

    x += zhuYinFontMetrics.width(m_tone) * 2 / 3;
    // Align the tone to half way between the last 2 zhu yin characters
    yOffset = yOffset + zhuYinFontHeight / 2 + zhuYinFontHeight * (m_zhuYin.length() - 2);
    painter.drawText(x, yOffset, m_tone);

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

void ZhChar::parseZhuYin(const QString &zhuYin)
{
    if (!zhuYin.isEmpty())  {
        m_zhuYin = zhuYin;
        m_tone = zhuYin.at(zhuYin.length() - 1);
        if (m_tones.contains(m_tone)) {
            m_zhuYin.chop(1);
        } else
            m_tone = std::move(QChar());
    }
}

void ZhChar::setZhuYin(const QString &zhuYin)
{
    parseZhuYin(zhuYin);
}

QChar ZhChar::tone() const
{
    return m_tone;
}

QChar ZhChar::zhChar() const
{
    return m_char;
}

QString ZhChar::zhuYin() const
{
    return m_zhuYin;
}
