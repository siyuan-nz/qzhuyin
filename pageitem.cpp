#include "pageitem.h"

#include "pageitemvisitor.h"

#define CLASSNAME(name) \
QString name::className() const { \
    return #name; \
}

PageItem::~PageItem()
{
}

CLASSNAME(Box)

void Box::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(LineText)

void LineText::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}
