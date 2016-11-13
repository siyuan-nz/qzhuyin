#ifndef PAGEITEM_H
#define PAGEITEM_H

#include "zhchar.h"

#include <QList>
#include <QString>

class AstVisitor;

class AstNode
{
public:
    virtual ~AstNode();
    virtual void welcome(AstVisitor &visitor) = 0;
};

class HSpace : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;

    int m_space;
};

class VSpace : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;

    int m_space;
};

class NewLine : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;
};

class NewPage : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;
};

class NewParagraph : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;
};

class Scope : public AstNode
{
public:
    ~Scope();
    void welcome(AstVisitor &visitor) override;

    QList<AstNode *> m_childNodes;
};

class SetFont : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;

    QString m_family;
};

class SetFontSize : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;

    int m_pointSize;
};

class SetPageMargin : public AstNode
{
public:
    int m_mm;
protected:
    SetPageMargin() {}
};

class SetBottomMargin : public SetPageMargin
{
public:
    void welcome(AstVisitor &visitor) override;
};

class SetLeftMargin : public SetPageMargin
{
public:
    void welcome(AstVisitor &visitor) override;
};

class SetRightMargin : public SetPageMargin
{
public:
    void welcome(AstVisitor &visitor) override;
};

class SetTopMargin : public SetPageMargin
{
public:
    void welcome(AstVisitor &visitor) override;
};

class Text : public AstNode
{
public:
    void welcome(AstVisitor &visitor) override;

    QList<ZhChar> m_text;
};

class AstNodeRef : public AstNode
{
public:
    AstNodeRef(AstNode &ref);

    AstNode &m_astNode;
};

class ScopeRef : public AstNodeRef
{
public:
    ScopeRef(Scope &scope, int position);
    void welcome(AstVisitor &visitor) override;

    int m_position;
};

class TextRef : public AstNodeRef
{
public:
    TextRef(Text &text, int position);
    void welcome(AstVisitor &visitor) override;

    int m_position;
};

#endif
