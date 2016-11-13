#ifndef ZHCHAR_H
#define ZHCHAR_H

#include <QString>

class QPainter;

class ZhChar
{
public:
    ZhChar(const QChar &zhChar, const QString &zhuYin = QString());
    ZhChar(const ZhChar &other);
    ZhChar(ZhChar &&other);

    void draw(QPainter &painter, int x, int y);
    void setZhuYin(const QString &zhuyin);
    QChar tone() const;
    QChar zhChar() const;
    QString zhuYin() const;

    ZhChar& operator=(const ZhChar &other);
    ZhChar& operator=(ZhChar &&other);

    static void drawText(QPainter &painter, int x, int y, const QString &text);

private:
    void parseZhuYin(const QString &zhuYin);

    QChar m_char;
    QChar m_tone;
    QString m_zhuYin;

    static const QString m_tones;
};

#endif
