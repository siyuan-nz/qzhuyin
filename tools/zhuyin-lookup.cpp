#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QTextStream>
#include <QDebug>

#include <iostream>

typedef QHash<QChar, QString> ZhuYinMap;

void loadDb(ZhuYinMap &map)
{
    QFile zhuYinDbFile(":zhuyin_db_sorted.txt");

    if (!zhuYinDbFile.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal("Failed to open DB");

    QTextStream textStream(&zhuYinDbFile);

    while (!textStream.atEnd()) {
        QString character;
        QString zhuYin;
        textStream >> character >> zhuYin;
        if (!character.isEmpty())
            map.insertMulti(character.at(0), zhuYin);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ZhuYinMap map;
    loadDb(map);

    QFile stdIn;
    stdIn.open(stdin, QIODevice::ReadOnly | QIODevice::Text);

    QByteArray buffer;
    while (!(buffer = stdIn.read(4096)).isEmpty()) {
        QString stringBuffer = QString::fromUtf8(buffer);
        bool writeOut = false;
        QString text;
        QStringList zhuYinList;

        for (int i = 0; i < stringBuffer.length(); i++) {
            QChar c = stringBuffer.at(i);

            if (c.isSpace() || c.isPunct()) {
                writeOut = true;
            } else {
                QStringList result = map.values(c);
                if (!result.isEmpty())
                    zhuYinList += result.join('|');
            }

            if (writeOut || i >= (stringBuffer.length() - 1)) {
                if (!zhuYinList.isEmpty())
                    text += "\\zhuyin{" + zhuYinList.join(' ') + '}';

                std::cout << qPrintable(text);
                text.clear();
                zhuYinList.clear();
                writeOut = false;
            }

            text += c;
        }
    }

    return 0;
}
