#ifndef ZHCHAR_H
#define ZHCHAR_H

#include <QString>

class QPainter;

class ZhChar
{
public:
    ZhChar(const QChar &zhChar, const QString &zhuYin);

    QChar zhChar() const { return m_Char; }
    QString zhuYin() const { return m_ZhuYin; }
    QChar tone() const { return m_Tone; }
    void draw(QPainter &painter, int x, int y);

    static void drawText(QPainter &painter, int x, int y, const QString &text);

private:
    QChar m_Char;
    QString m_ZhuYin;
    QChar m_Tone;

    static const QString m_Tones;
};

#endif
