#ifndef QZY2AST_H
#define QZY2AST_H

#include <functional>

#include <QHash>
#include <QStack>
#include <QString>

class AstNode;
class Label;
class Scope;

enum class eCommandStatus {
    Success,
    Warning,
    Error
};

struct CommandStatus
{
    CommandStatus();
    CommandStatus(CommandStatus &&other);
    CommandStatus& operator=(CommandStatus &&other);

    eCommandStatus status;
    QString message;
};

class Qzy2Ast
{
public:
    Qzy2Ast(const QString &fileName);
    AstNode* parse();

private:
    CommandStatus hSpace(const QString &arg);
    CommandStatus vSpace(const QString &arg);
    CommandStatus label(const QString &arg);
    CommandStatus newPage(const QString &arg);
    CommandStatus ref(const QString &arg);
    CommandStatus setBottomMargin(const QString &arg);
    CommandStatus setLeftMargin(const QString &arg);
    CommandStatus setRightMargin(const QString &arg);
    CommandStatus setTopMargin(const QString &arg);
    CommandStatus setFont(const QString &arg);
    CommandStatus setFontSize(const QString &arg);
    CommandStatus zhuYin(const QString &arg);

    int parseSingleIntArg(CommandStatus &status, const QString &commandName, const QString &arg, bool positiveOnly = false);
    void appendWarningMessage(CommandStatus &status, const QString &message);

    enum class eParseState {
        CommandArguments,
        CommandEnd,
        CommandName,
        CommandNameEnd,
        CommandOptions,
        CommandOptionsEnd,
        Comment,
        Escape,
        SkipWhiteSpace,
        Text,
    } m_parseState = eParseState::Text;

    const QHash<QString, std::function<CommandStatus(const QString &)> > m_cCmd2Func;
    QString m_fileName;
    QStack<Scope *> m_scopeStack;
    QHash<QString, Label *> m_namedLabelHash;
    QList<Label *> m_unreferencedLabels;
};

#endif
