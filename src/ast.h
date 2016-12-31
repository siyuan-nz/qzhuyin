#ifndef AST_H
#define AST_H

#include "zhchar.h"

#include <QList>
#include <QString>

class AstVisitor;

class AstNode
{
public:
    virtual ~AstNode();
    virtual QString className() const = 0;
    virtual void welcome(AstVisitor &visitor) = 0;
};

class HSpace : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    int m_space;
};

class VSpace : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    int m_space;
};

class Label : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    int m_id;
    QString m_label;
};

class Ref : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    Label *m_labelNode = nullptr;
};

class NewLine : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class NewPage : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class NewParagraph : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class Scope : public AstNode
{
public:
    ~Scope();
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    QList<AstNode *> m_childNodes;
};

class SetFont : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    QString m_family;
};

class SetFontSize : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    int m_pointSize;
};

class SetPageMargin : public AstNode
{
public:
    int m_mm;
};

class SetBottomMargin : public SetPageMargin
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class SetLeftMargin : public SetPageMargin
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class SetRightMargin : public SetPageMargin
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class SetTopMargin : public SetPageMargin
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class Text : public AstNode
{
public:
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    QList<ZhChar> m_text;
};

class AstNodeRef : public AstNode
{
public:
    AstNodeRef(AstNode &ref);

    AstNode &m_astNode;
};

class NewParagraphRef : public AstNodeRef
{
public:
    using AstNodeRef::AstNodeRef;
    QString className() const override;
    void welcome(AstVisitor &visitor) override;
};

class ScopeRef : public AstNodeRef
{
public:
    ScopeRef(Scope &scope, int position);
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    int m_position;
};

class TextRef : public AstNodeRef
{
public:
    TextRef(Text &text, int position);
    QString className() const override;
    void welcome(AstVisitor &visitor) override;

    int m_position;
};

#endif
