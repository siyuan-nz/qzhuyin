#include "ast.h"
#include "astvisitor.h"

AstNode::~AstNode()
{
}

void HSpace::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void VSpace::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void NewLine::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void NewPage::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void NewParagraph::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

Scope::~Scope()
{
    while (!m_childNodes.isEmpty()) {
        delete m_childNodes.takeFirst();
    }
}

void Scope::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void SetFont::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void SetFontSize::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void SetBottomMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void SetLeftMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void SetRightMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void SetTopMargin::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

void Text::welcome(AstVisitor &visitor)
{
    visitor.visit(*this);
}

AstNodeRef::AstNodeRef(AstNode& ref)
    : m_astNode(ref)
{
}

ScopeRef::ScopeRef(Scope& scope, int position)
    : AstNodeRef(scope)
    , m_position(position)
{
}

void ScopeRef::welcome(AstVisitor& visitor)
{
    visitor.visit(*this);
}

TextRef::TextRef(Text& text, int position)
    : AstNodeRef(text)
    , m_position(position)
{
}

void TextRef::welcome(AstVisitor& visitor)
{
    visitor.visit(*this);
}
