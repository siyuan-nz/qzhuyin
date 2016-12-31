#include "ast.h"
#include "astvisitor.h"

#define CLASSNAME(name) \
QString name::className() const { \
    return #name; \
}

AstNode::~AstNode()
{
}

CLASSNAME(HSpace)

void HSpace::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(VSpace)

void VSpace::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(Label)

void Label::welcome(AstVisitor& visitor)
{
    visitor.visit(*this);
}

CLASSNAME(Ref)

void Ref::welcome(AstVisitor& visitor)
{
    visitor.visit(*this);
}

CLASSNAME(NewLine)

void NewLine::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(NewPage)

void NewPage::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(NewParagraph)

void NewParagraph::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

Scope::~Scope()
{
    while (!m_childNodes.isEmpty())
        delete m_childNodes.takeFirst();
}

CLASSNAME(Scope)

void Scope::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(SetFont)

void SetFont::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(SetFontSize)

void SetFontSize::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(SetBottomMargin)

void SetBottomMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(SetLeftMargin)

void SetLeftMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(SetRightMargin)

void SetRightMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(SetTopMargin)

void SetTopMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

CLASSNAME(Text)

void Text::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

AstNodeRef::AstNodeRef(AstNode &ref)
    : m_astNode(ref)
{
}

CLASSNAME(NewParagraphRef)

void NewParagraphRef::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

ScopeRef::ScopeRef(Scope& scope, int position)
    : AstNodeRef(scope)
    , m_position(position)
{
}

CLASSNAME(ScopeRef)

void ScopeRef::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

TextRef::TextRef(Text& text, int position)
    : AstNodeRef(text)
    , m_position(position)
{
}

CLASSNAME(TextRef)

void TextRef::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}
