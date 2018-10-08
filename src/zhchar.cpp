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

ZhChar::ZhChar(ZhChar &&other) noexcept
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

ZhChar& ZhChar::operator=(ZhChar &&other) noexcept
{
    m_char = std::move(other.m_char);
    m_tone = std::move(other.m_tone);
    m_zhuYin = std::move(other.m_zhuYin);
    return *this;
}

void ZhChar::parseZhuYin(const QString &zhuYin)
{
    if (!zhuYin.isEmpty())  {
        m_zhuYin = zhuYin;
        m_tone = zhuYin.at(zhuYin.length() - 1);
        if (m_tones.contains(m_tone)) {
            m_zhuYin.chop(1);
        } else
            m_tone = QChar();
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
