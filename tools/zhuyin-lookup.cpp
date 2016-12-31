#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QTextStream>

#include <iostream>

typedef QHash<QChar, QString> ZhuYinMap;

struct Options {
    QString fileName;
};

void loadDb(ZhuYinMap &map)
{
    QFile zhuYinDbFile(QStringLiteral(":zhuyin_db_sorted.txt"));

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

void parseOptions(const QCoreApplication &app, Options &options)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Chinese phonetic guide insertion tool for qzhuyin"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("text_file"), QCoreApplication::translate("main", "Text file."));
    parser.process(app);

    QStringList positionalArguments = parser.positionalArguments();
    if (!positionalArguments.isEmpty())
        options.fileName = positionalArguments.first();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Options options;
    parseOptions(app, options);

    ZhuYinMap map;
    loadDb(map);

    QFile sourceFile;
    if (options.fileName.isEmpty() || options.fileName == QStringLiteral("-"))
        sourceFile.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
    else {
        sourceFile.setFileName(options.fileName);
        if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("Unable to open %s", qPrintable(options.fileName));
            exit(1);
        }
    }

    QByteArray buffer;
    while (!(buffer = sourceFile.read(4096)).isEmpty()) {
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
                    text += QStringLiteral("\\zhuyin{") + zhuYinList.join(' ') + '}';

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
