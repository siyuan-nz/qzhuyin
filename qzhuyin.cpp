#include <QtWidgets>
#include <QtGui>
#include <QDebug>

void createPDF(QString a_strFileName)
{
    QPdfWriter writer(a_strFileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QFont pmingliu(QFont("PMingLiu", 16), &writer);
    QFontMetrics fontMetrics(pmingliu, &writer);
    QPainter painter(&writer);
    painter.setFont(pmingliu);
    QString text("中文直書測試，含注音輔助。");
    int x = writer.width() - fontMetrics.width(text, 1) * 2;
    int y = fontMetrics.height() * 3;
    for (auto c : text)
    {
        painter.drawText(x, y, c);
        y += fontMetrics.height();
    }

    painter.end();
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
