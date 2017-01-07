#include "pageitem.h"
#include "common.h"
#include "pageitemvisitor.h"

PageItem::~PageItem()
{
}

Box::~Box()
{
    while (!m_enclosedItems.isEmpty())
        delete m_enclosedItems.takeFirst();
}

CLASSNAME(Box)

void Box::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(EllipsisText)

void EllipsisText::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(LabelText)

void LabelText::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(LineText)

void LineText::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}
