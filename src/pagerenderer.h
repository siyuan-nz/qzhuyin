#ifndef PAGERENDERER_H
#define PAGERENDERER_H

#include "pageitemvisitor.h"

#include <QPainter>

class Page;
class QPdfWriter;
class ZhChar;

class PageRenderer : protected PageItemVisitor
{
public:
    PageRenderer(QPdfWriter &pdfWriter);
    QString className() const override;
    void render(Page &page);

protected:
    void visit(Box &) override;
    void visit(EllipsisText &) override;
    void visit(LabelText &) override;
    void visit(LineText &) override;

private:
    void drawZhChar(int x, int y, const ZhChar &zhChar);
    void drawText(int x, int y, const QString &text);

    QPdfWriter &m_pdfWriter;
    QPainter m_painter;
};

#endif
