#include "astprinter.h"
#include "ast.h"
#include "common.h"

#include <iostream>

CLASSNAME(AstPrinter)

AstPrinter::AstPrinter(AstNode &root)
{
    root.welcome(*this);
}

void AstPrinter::printIndent()
{
    for (int i = 0; i < m_indentLevel; i++)
        std::cout << "    ";
}

void AstPrinter::printLine(const QString &message)
{
    printIndent();
    std::cout << qPrintable(message) << std::endl;
}

void AstPrinter::visit(Ellipsis &astNode)
{
    QString message = astNode.className() + QStringLiteral(": length = ") + QString::number(astNode.m_length);
    printLine(message);
}

void AstPrinter::visit(HSpace &astNode)
{
    QString message = astNode.className() + QStringLiteral(": space = ") + QString::number(astNode.m_space);
    printLine(message);
}

void AstPrinter::visit(VSpace &astNode)
{
    QString message = astNode.className() + QStringLiteral(": space = ") + QString::number(astNode.m_space);
    printLine(message);
}

void AstPrinter::visit(Label &astNode)
{
    QString message = astNode.className() + QStringLiteral(": label = ") + astNode.m_label;
    printLine(message);
}

void AstPrinter::visit(Ref &astNode)
{
    if (astNode.m_labelNode == nullptr)
        qFatal("Cannot find the corresponding \\label for \\ref");

    QString message = astNode.className() + QStringLiteral(": label = ") + astNode.m_labelNode->m_label;
    printLine(message);
}

void AstPrinter::visit(NewLine &astNode)
{
    printLine(astNode.className());
}

void AstPrinter::visit(NewPage &astNode)
{
    printLine(astNode.className());
}

void AstPrinter::visit(NewParagraph &astNode)
{
    printLine(astNode.className());
}

void AstPrinter::visit(Scope &astNode)
{
    QString message = astNode.className() + ':';
    printLine(message);

    m_indentLevel++;
    for (auto node : astNode.m_childNodes)
        node->welcome(*this);
    m_indentLevel--;
}

void AstPrinter::visit(SetBottomMargin &astNode)
{
    QString message = astNode.className() + QStringLiteral(": margin = ") + QString::number(astNode.m_mm) + QStringLiteral("mm");
    printLine(message);
}

void AstPrinter::visit(SetLeftMargin &astNode)
{
    QString message = astNode.className() + QStringLiteral(": margin = ") + QString::number(astNode.m_mm) + QStringLiteral("mm");
    printLine(message);
}

void AstPrinter::visit(SetRightMargin &astNode)
{
    QString message = astNode.className() + QStringLiteral(": margin = ") + QString::number(astNode.m_mm) + QStringLiteral("mm");
    printLine(message);
}

void AstPrinter::visit(SetTopMargin &astNode)
{
    QString message = astNode.className() + QStringLiteral(": margin = ") + QString::number(astNode.m_mm) + QStringLiteral("mm");
    printLine(message);
}

void AstPrinter::visit(SetFont &astNode)
{
    QString message = astNode.className() + QStringLiteral(": font = ") + astNode.m_family;
    printLine(message);
}

void AstPrinter::visit(SetFontSize &astNode)
{
    QString message = astNode.className() + QStringLiteral(": size = ") + QString::number(astNode.m_pointSize) + QStringLiteral(" pt");
    printLine(message);
}

void AstPrinter::visit(Text &astNode)
{
    QString message = astNode.className() + QStringLiteral(": ");

    for (const ZhChar &zhChar : astNode.m_text) {
        message += QString(zhChar.zhChar());

        if (!zhChar.zhuYin().isEmpty()) {
            message += '(' + zhChar.zhuYin();

            if (!zhChar.tone().isNull())
                message += zhChar.tone();

            message += QStringLiteral(") ");
        }
    }

    printLine(message);
}

void AstPrinter::visit(EllipsisRef &astNode)
{
    QString message = astNode.className() + QStringLiteral(": position = ") + QString::number(astNode.m_position);
    printLine(message);
}

void AstPrinter::visit(NewParagraphRef &astNode)
{
    printLine(astNode.className());
}

void AstPrinter::visit(ScopeRef &astNode)
{
    QString message = astNode.className() + QStringLiteral(": position = ") + QString::number(astNode.m_position);
    printLine(message);
}

void AstPrinter::visit(TextRef &astNode)
{
    Text &textNode = static_cast<Text &>(astNode.m_astNode);
    QString message = astNode.className() + QStringLiteral(": position = ") + QString::number(astNode.m_position) + QStringLiteral(": ");

    for (int i = astNode.m_position; i < textNode.m_text.count(); i++) {
        const ZhChar &zhChar = textNode.m_text.at(i);
        message += QString(zhChar.zhChar());

        if (!zhChar.zhuYin().isEmpty()) {
            message += '(' + zhChar.zhuYin();

            if (!zhChar.tone().isNull())
                message += zhChar.tone();

            message += QStringLiteral(") ");
        }
    }

    printLine(message);
}
