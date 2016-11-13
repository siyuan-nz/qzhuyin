#include <QtWidgets>
#include <QtGui>
#include <QDebug>
#include <QScopedPointer>

#include "ast.h"
#include "astprinter.h"
#include "qzy2ast.h"
#include "zhchar.h"

void createPDF(QString a_strFileName)
{
    QPageLayout pageLayout(QPageSize(QPageSize::A4),
                           QPageLayout::Portrait,
                           QMarginsF(20, 20, 20, 20),
                           QPageLayout::Millimeter);
    QPdfWriter writer(a_strFileName);
    writer.setPageLayout(pageLayout);
    QFont pmingliu(QFont("文鼎ＰＬ中楷", 24), &writer);
    QFont zhuyinFont(QFont("文鼎ＰＬ中楷", 5), &writer);
    QFontMetrics fontMetrics(pmingliu, &writer);
    QFontMetrics zhuyinMetrics(zhuyinFont, &writer);
    QPainter painter(&writer);
qDebug() << "pageLayout =" << writer.pageLayout();
qDebug() << "paintRect =" << writer.pageLayout().paintRectPoints();
qDebug() << "viewport =" << painter.viewport();
    painter.setFont(pmingliu);
    QString text("初，鄭武公娶于申，曰武姜，生莊公及共叔段。\n莊公寤生，驚姜氏，故名曰寤生，遂惡之。愛共叔段，欲立之。亟請於武公，公弗許。\n\n及莊公即位，為之請制。公曰：﹁制，巖邑也。虢叔死焉，佗邑唯命。﹂請京，使居之，謂之京城大叔。");
    QStringList zhuyin{"ㄔㄨ", "", "ㄓㄥˋ", "ㄨˇ", "ㄍㄨㄥ", "ㄑㄩˇ", "ㄩˊ", "ㄕㄣ", "", "ㄩㄝ", "ㄨˇ", "ㄐㄧㄤ", "", "ㄕㄥ", "ㄓㄨㄤ", "ㄍㄨㄥ", "ㄐㄧˊ", "ㄍㄨㄥ", "ㄕㄨˊ", "ㄉㄨㄢˋ", "", "", "ㄓㄨㄤ", "ㄍㄨㄥ", "ㄨˋ", "ㄕㄥ", "", "ㄐㄧㄥ", "ㄐㄧㄤ", "ㄕˋ", "", "ㄍㄨˋ", "ㄇㄧㄥˊ", "ㄩㄝ", "ㄨˋ", "ㄕㄥ", "", "ㄙㄨㄟˋ", "ㄨˋ", "ㄓ", "", "ㄞˋ", "ㄍㄨㄥ", "ㄕㄨˊ", "ㄉㄨㄢˋ", "", "ㄩˋ", "ㄌㄧˋ", "ㄓ", "", "ㄑㄧˋ", "ㄑㄧㄥˇ", "ㄩˊ", "ㄨˇ", "ㄍㄨㄥ", "", "ㄍㄨㄥ", "ㄈㄨˊ", "ㄒㄩˇ", "", "", "", "ㄐㄧˊ", "ㄓㄨㄤ", "ㄍㄨㄥ", "ㄐㄧˊ", "ㄨㄟˋ", "", "ㄨㄟˋ", "ㄓ", "ㄑㄧㄥˇ", "ㄓˋ", "", "ㄍㄨㄥ", "ㄩㄝ", "", "", "ㄓˋ", "", "ㄧㄢˊ", "ㄧˋ", "ㄧㄝˇ", "", "ㄍㄨㄛˊ", "ㄕㄨˊ", "ㄙˇ", "ㄧㄢ", "", "ㄊㄨㄛ", "ㄧˋ", "ㄨㄟˊ", "ㄇㄧㄥˋ", "", "", "ㄑㄧㄥˇ", "ㄐㄧㄥ", "", "ㄕˇ", "ㄐㄩ", "ㄓ", "", "ㄨㄟˋ", "ㄓ", "ㄐㄧㄥ", "ㄔㄥˊ", "ㄊㄞˋ", "ㄕㄨˊ", ""};
    int x = writer.width() - fontMetrics.width(text, 1) - zhuyinMetrics.width(zhuyin.first(), 1) * 3;
    int y = 0;
    int i = 0;
    int newLines = 0;
    for (auto c : text) {
        ZhChar zhChar(c, zhuyin.at(i));
qDebug() << "i =" << i << ", c =" << QString(c) << ", zhuyin =" << zhuyin.at(i);
        if (c == '\n') {
            newLines++;
            y = 0;
            if (newLines > 1) {
                x -= fontMetrics.width(text, 1);
                newLines = 0;
            } else {
                x -= fontMetrics.width(text, 1) * 5 / 3;
            }
            i++;
            continue;
        }
        zhChar.draw(painter, x, y);
        y += fontMetrics.lineSpacing();
        if (y > painter.viewport().height()) {
            y = 0;
                x -= fontMetrics.width(text, 1) * 5 / 3;
        }
        i++;
    }
}

AstNode* testParser(const QString &fileName)
{
    Qzy2Ast qzy2Ast(fileName);
    QScopedPointer<AstNode> xRootNode(qzy2Ast.parse());
    AstPrinter astPrinter(*xRootNode);
    return xRootNode.take();
}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
//     createPDF("output.pdf");
    QScopedPointer<AstNode> xRootNode;
    xRootNode.reset(testParser("example.qzy"));
//     QProcess okular;
//     okular.start("okular output.pdf");
//     okular.waitForFinished(-1);
    return 0;
}
