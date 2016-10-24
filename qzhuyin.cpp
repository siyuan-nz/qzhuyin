#include <QtWidgets>
#include <QtGui>
#include <QDebug>

#include "zhchar.h"

void createPDF(QString a_strFileName)
{
    QPageLayout pageLayout(QPageSize(QPageSize::A4),
                           QPageLayout::Portrait,
                           QMarginsF(20, 20, 20, 20),
                           QPageLayout::Millimeter);
    QPdfWriter writer(a_strFileName);
    writer.setPageLayout(pageLayout);
    QFont pmingliu(QFont("文鼎ＰＬ中楷", 12), &writer);
    QFont zhuyinFont(QFont("文鼎ＰＬ中楷", 5), &writer);
    QFontMetrics fontMetrics(pmingliu, &writer);
    QFontMetrics zhuyinMetrics(zhuyinFont, &writer);
    QPainter painter(&writer);
    painter.setFont(pmingliu);
    QString text("中文直書測試，含注音輔助。");
    QStringList zhuyin{"ㄓㄨㄥ", "ㄨㄣˊ", "ㄓˊ", "ㄕㄨ", "ㄘㄜˋ", "ㄕˋ", "", "ㄏㄢˊ", "ㄓㄨˋ", "ㄧㄣ", "ㄈㄨˇ", "ㄓㄨˋ", ""};
    int x = writer.width() - fontMetrics.width(text, 1) - zhuyinMetrics.width(zhuyin.first(), 1);
    int y = 0;
    int i = 0;
    for (auto c : text) {
        ZhChar zhChar(c, zhuyin.at(i));
        zhChar.draw(painter, x, y);
        y += fontMetrics.lineSpacing();
        i++;
    }
}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    createPDF("output.pdf");
    QProcess okular;
    okular.start("okular output.pdf");
    okular.waitForFinished(-1);
    return 0;
}
