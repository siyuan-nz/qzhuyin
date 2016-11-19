#include "astprinter.h"
#include "ast.h"

#include <iostream>

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

void AstPrinter::visit(HSpace &astNode)
{
    QString message = astNode.className() + ": space = " + QString::number(astNode.m_space);
    printLine(message);
}

void AstPrinter::visit(VSpace &astNode)
{
    QString message = astNode.className() + ": space = " + QString::number(astNode.m_space);
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
    QString message = astNode.className() + ": margin = " + QString::number(astNode.m_mm) + "mm";
    printLine(message);
}

void AstPrinter::visit(SetLeftMargin &astNode)
{
    QString message = astNode.className() + ": margin = " + QString::number(astNode.m_mm) + "mm";
    printLine(message);
}

void AstPrinter::visit(SetRightMargin &astNode)
{
    QString message = astNode.className() + ": margin = " + QString::number(astNode.m_mm) + "mm";
    printLine(message);
}

void AstPrinter::visit(SetTopMargin &astNode)
{
    QString message = astNode.className() + ": margin = " + QString::number(astNode.m_mm) + "mm";
    printLine(message);
}

void AstPrinter::visit(SetFont &astNode)
{
    QString message = astNode.className() + ": font = " + astNode.m_family;
    printLine(message);
}

void AstPrinter::visit(SetFontSize &astNode)
{
    QString message = astNode.className() + ": size = " + QString::number(astNode.m_pointSize) + " pt";
    printLine(message);
}

void AstPrinter::visit(Text &astNode)
{
    QString message = astNode.className() + ": ";

    for (const ZhChar &zhChar : astNode.m_text) {
        message += QString(zhChar.zhChar());

        if (!zhChar.zhuYin().isEmpty()) {
            message += '(' + zhChar.zhuYin();

            if (!zhChar.tone().isNull())
                message += zhChar.tone();

            message += ") ";
        }
    }

    printLine(message);
}

void AstPrinter::visit(ScopeRef &astNode)
{
    QString message = astNode.className() + ": position = " + QString::number(astNode.m_position);
    printLine(message);
}

void AstPrinter::visit(TextRef &astNode)
{
    QString message = astNode.className() + ": position = " + QString::number(astNode.m_position);
    printLine(message);
}
