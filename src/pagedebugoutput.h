#ifndef PAGEDEBUGOUTPUT_H
#define PAGEDEBUGOUTPUT_H

#include "pageitemvisitor.h"

#include <QString>

class Box;
class LineText;
class Page;

class PageDebugOutput : public PageItemVisitor
{
public:
    PageDebugOutput(Page &page);
    QString className() const override;

protected:
    void visit(Box &) override;
    void visit(EllipsisText &) override;
    void visit(LabelText &) override;
    void visit(LineText &) override;

private:
    QString indent() const;

    int m_indentLevel = 0;
};

#endif
