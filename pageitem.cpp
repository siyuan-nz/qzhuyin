#include "pageitem.h"

#include "pageitemvisitor.h"

void Box::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}

void LineText::welcome(PageItemVisitor &visitor)
{
    visitor.visit(*this);
}
