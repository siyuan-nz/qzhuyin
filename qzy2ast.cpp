#include "qzy2ast.h"
#include "ast.h"

#include <QFile>

using namespace std::placeholders;

CommandStatus::CommandStatus()
{
}

CommandStatus::CommandStatus(CommandStatus && other)
    : status(other.status)
    , message(std::move(other.message))
{
}

CommandStatus & CommandStatus::operator=(CommandStatus && other)
{
    status = other.status;
    message = std::move(other.message);
    return *this;
}

Qzy2Ast::Qzy2Ast(const QString &fileName)
    : m_cCmd2Func{
          { "hspace", std::bind(&Qzy2Ast::hSpace, this, _1) },
          { "newpage", std::bind(&Qzy2Ast::newPage, this, _1) },
          { "setbottommargin", std::bind(&Qzy2Ast::setBottomMargin, this, _1) },
          { "setfont", std::bind(&Qzy2Ast::setFont, this, _1) },
          { "setfontsize", std::bind(&Qzy2Ast::setFontSize, this, _1) },
          { "setleftmargin", std::bind(&Qzy2Ast::setLeftMargin, this, _1) },
          { "setrightmargin", std::bind(&Qzy2Ast::setRightMargin, this, _1) },
          { "settopmargin", std::bind(&Qzy2Ast::setTopMargin, this, _1) },
          { "vspace", std::bind(&Qzy2Ast::vSpace, this, _1) },
          { "zhuyin", std::bind(&Qzy2Ast::zhuYin, this, _1) },
      }
    , m_fileName(fileName)
{
}

AstNode* Qzy2Ast::parse()
{
    QFile qzyFile(m_fileName);

    if (!qzyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qFatal("Unable to open %s: error %d", qPrintable(m_fileName), qzyFile.error());
        return nullptr;
    }

    m_scopeStack.push(new Scope);

    QByteArray buffer;
    NewParagraph *pParagraph = nullptr;
    Text *pCurrentText = nullptr;
    VSpace *pVSpace = nullptr;
    unsigned int line = 1;
    unsigned int column = 1;
    unsigned int lineFeeds = 0;
    bool escaped = false;
    QString commandName;
    QString commandArguments;
    auto addNonTextNode = [&pCurrentText, this](AstNode *pNode) {
        if (pNode)
            m_scopeStack.top()->m_childNodes.append(pNode);
        pCurrentText = nullptr;
    };
    auto onWarning = [&line, &column, this](const QString &msg) {
        qWarning("%s:%u,%u: warning: %s", qPrintable(m_fileName), line, column, qPrintable(msg));
    };
    auto onError = [&line, &column, this](const QString &msg) {
        qFatal("%s:%u,%u: error: %s", qPrintable(m_fileName), line, column, qPrintable(msg));
    };

    while (!(buffer = qzyFile.read(4096)).isEmpty()) {
        QString stringBuffer = QString::fromUtf8(buffer);

        for (auto c : stringBuffer) {
            column++;

            if (c == '\n') {
                line++;
                column = 1;
            }

            bool stateLoop;
            do {
                stateLoop = false;

                switch (m_parseState) {
                case eParseState::CommandArguments:
                    if (c == '\n') {
                        lineFeeds++;
                        if (lineFeeds > 1) {
                            m_parseState = eParseState::CommandEnd;
                            stateLoop = true;
                        }
                    } else if (c == '}') {
                        m_parseState = eParseState::CommandEnd;
                        stateLoop = true;
                    } else
                        commandArguments += c;
                    break;

                case eParseState::CommandEnd:
                    {
                        auto funcIter = m_cCmd2Func.find(commandName);
                        if (funcIter == m_cCmd2Func.end())
                            onError(QString("unknown command ") + commandName);

                        commandArguments = commandArguments.simplified();
                        CommandStatus &&status = (*funcIter)(commandArguments);
                        switch (status.status) {
                        case eCommandStatus::Success:
                            pCurrentText = nullptr;
                            break;
                        case eCommandStatus::Warning:
                            onWarning(status.message);
                            break;
                        case eCommandStatus::Error:
                            onError(status.message);
                            break;
                        }

                        m_parseState = eParseState::Text;
                    }
                    break;

                case eParseState::CommandName:
                    if (c.isSpace() || c.isPunct()) {
                        m_parseState = eParseState::CommandNameEnd;
                        stateLoop = true;
                        break;
                    }
                    commandName += c;
                    break;

                case eParseState::CommandNameEnd:
                    if (c == '[') {
                        lineFeeds = 0;
                        m_parseState = eParseState::CommandOptions;
                    } else if (c == '{') {
                        lineFeeds = 0;
                        m_parseState = eParseState::CommandArguments;
                        commandArguments.clear();
                    } else if (c == '\n') {
                        lineFeeds++;
                        if (lineFeeds > 1)
                            onError("a new paragraph started before the command ends");
                    } else if (!c.isSpace())
                        onError("command ended prematurely");
                    break;

                case eParseState::CommandOptions:
                case eParseState::CommandOptionsEnd:
                    onError("command options are unsupported yet");
                    break;

                case eParseState::Comment:
                    if (c == '\n')
                        m_parseState = eParseState::Text;
                    break;

                case eParseState::Escape:
                    if (c.isSpace())
                        onError("incomplete escape sequence");
                    else if (c == '\\') {
                        addNonTextNode(new NewLine);
                        m_parseState = eParseState::Text;
                    } else if (c.isPunct()) {
                        escaped = true;
                        m_parseState = eParseState::Text;
                        stateLoop = true;
                    } else {
                        commandName.clear();
                        m_parseState = eParseState::CommandName;
                        stateLoop = true;
                    }
                    break;

                case eParseState::SkipWhiteSpace:
                    if (c == '\n')
                        lineFeeds++;
                    else if (!c.isSpace()) {
                        if (lineFeeds > 1)
                            pParagraph = nullptr;
                        else {
                            pVSpace = new VSpace;
                            pVSpace->m_space = 1;
                            addNonTextNode(pVSpace);
                        }

                        m_parseState = eParseState::Text;
                        stateLoop = true;
                    }
                    break;

                case eParseState::Text:
                    if (escaped)
                        escaped = false;
                    else {
                        if (c == '%') {
                            m_parseState = eParseState::Comment;
                            break;
                        } else if (c == '\\') {
                            pCurrentText = nullptr;
                            m_parseState = eParseState::Escape;
                            break;
                        } else if (c == '{') {
                            pCurrentText = nullptr;
                            Scope *pScope = new Scope;
                            addNonTextNode(pScope);
                            m_scopeStack.push(pScope);
                            break;
                        } else if (c == '}') {
                            pCurrentText = nullptr;
                            m_scopeStack.pop();

                            if (m_scopeStack.isEmpty())
                                onError("unmatched end of scope");

                            addNonTextNode(nullptr);
                            break;
                        } else if (c.isSpace()) {
                            lineFeeds = 0;
                            m_parseState = eParseState::SkipWhiteSpace;
                            stateLoop = true;
                            break;
                        }
                    }

                    if (pParagraph == nullptr) {
                        pParagraph = new NewParagraph;
                        addNonTextNode(pParagraph);
                    }

                    if (pCurrentText == nullptr) {
                        pCurrentText = new Text;
                        m_scopeStack.top()->m_childNodes.append(pCurrentText);
                    }

                    pCurrentText->m_text.append(ZhChar(c));
                    break;
                }
            } while (stateLoop);
        }
    }

    return m_scopeStack.pop();
}

int Qzy2Ast::parseSingleIntArg(CommandStatus &status, const QString &commandName, const QString &arg, bool positiveOnly)
{
    bool ok;
    bool failPositive = false;
    int value = arg.toInt(&ok);

    if (ok) {
        status.status = eCommandStatus::Success;

        if (positiveOnly && value < 0) {
            status.status = eCommandStatus::Error;
            failPositive = true;
        }
    }

    if (!ok || failPositive) {
        status.status = eCommandStatus::Error;
        status.message = QString("error parsing argument for \"")
                       + '\\' + commandName
                       + '{' + arg + "}\"";

        if (failPositive)
            status.message += ": argument must be a positive integer";
    }

    return value;
}

void Qzy2Ast::appendWarningMessage(CommandStatus &status, const QString &message)
{
    if (status.message.isEmpty())
        status.message = message;
    else {
        status.message += '\n' + message;
    }
}

CommandStatus Qzy2Ast::hSpace(const QString &arg)
{
    CommandStatus status;

    int space = parseSingleIntArg(status, "hspace", arg);
    if (status.status == eCommandStatus::Success) {
        HSpace *pHSpace = new HSpace;
        pHSpace->m_space = space;
        m_scopeStack.top()->m_childNodes.append(pHSpace);
    }

    return status;
}

CommandStatus Qzy2Ast::vSpace(const QString &arg)
{
    CommandStatus status;

    int space = parseSingleIntArg(status, "vspace", arg);
    if (status.status == eCommandStatus::Success) {
        VSpace *pVSpace = new VSpace;
        pVSpace->m_space = space;
        m_scopeStack.top()->m_childNodes.append(pVSpace);
    }

    return status;
}

CommandStatus Qzy2Ast::newPage(const QString &)
{
    CommandStatus status;
    m_scopeStack.top()->m_childNodes.append(new NewPage);
    status.status = eCommandStatus::Success;
    return status;
}

CommandStatus Qzy2Ast::setBottomMargin(const QString &arg)
{
    CommandStatus status;

    int mm = parseSingleIntArg(status, "setbottommargin", arg);
    if (status.status == eCommandStatus::Success) {
        SetBottomMargin *pMargin = new SetBottomMargin;
        pMargin->m_mm = mm;
        m_scopeStack.top()->m_childNodes.append(pMargin);
    }

    return status;
}

CommandStatus Qzy2Ast::setLeftMargin(const QString &arg)
{
    CommandStatus status;

    int mm = parseSingleIntArg(status, "setleftmargin", arg);
    if (status.status == eCommandStatus::Success) {
        SetLeftMargin *pMargin = new SetLeftMargin;
        pMargin->m_mm = mm;
        m_scopeStack.top()->m_childNodes.append(pMargin);
    }

    return status;
}

CommandStatus Qzy2Ast::setRightMargin(const QString &arg)
{
    CommandStatus status;

    int mm = parseSingleIntArg(status, "setrightmargin", arg);
    if (status.status == eCommandStatus::Success) {
        SetRightMargin *pMargin = new SetRightMargin;
        pMargin->m_mm = mm;
        m_scopeStack.top()->m_childNodes.append(pMargin);
    }

    return status;
}

CommandStatus Qzy2Ast::setTopMargin(const QString &arg)
{
    CommandStatus status;

    int mm = parseSingleIntArg(status, "settopmargin", arg);
    if (status.status == eCommandStatus::Success) {
        SetTopMargin *pMargin = new SetTopMargin;
        pMargin->m_mm = mm;
        m_scopeStack.top()->m_childNodes.append(pMargin);
    }

    return status;
}

CommandStatus Qzy2Ast::setFont(const QString &arg)
{
    CommandStatus status;
    SetFont *pSetFont = new SetFont;
    pSetFont->m_family = arg;
    m_scopeStack.top()->m_childNodes.append(pSetFont);
    status.status = eCommandStatus::Success;
    return status;
}

CommandStatus Qzy2Ast::setFontSize(const QString &arg)
{
    CommandStatus status;

    int pointSize = parseSingleIntArg(status, "setfontsize", arg, true);
    if (status.status == eCommandStatus::Success) {
        SetFontSize *pSetFontSize = new SetFontSize;
        pSetFontSize->m_pointSize = pointSize;
        m_scopeStack.top()->m_childNodes.append(pSetFontSize);
    }

    return status;
}

CommandStatus Qzy2Ast::zhuYin(const QString &arg)
{
    CommandStatus status;

    Text *pText = dynamic_cast<Text *>(m_scopeStack.top()->m_childNodes.last());
    if (pText) {
        QStringList &&zhuYinList = arg.split(' ', QString::SkipEmptyParts);
        bool prematureTerminate = false;
        auto textIter = pText->m_text.rbegin();
        auto textEnd = pText->m_text.rend();
        auto zhuYinEnd = zhuYinList.rend();

        for (auto zhuYinIter = zhuYinList.rbegin(); zhuYinIter != zhuYinEnd; zhuYinIter++) {
            if (textIter == textEnd) {
                prematureTerminate = true;
                break;
            } else {
                textIter->setZhuYin(*zhuYinIter);
                if (textIter->zhuYin().length() > 3) {
                    QString message = QString("invalid zhu yin for character \"")
                                    + textIter->zhChar() + "\" \"" + *zhuYinIter + '\"';
                    appendWarningMessage(status, message);
                }
            }

            textIter++;
        }

        if (prematureTerminate) {
            status.status = eCommandStatus::Warning;
            QString message("number of characters specified with \\zhuyin is more than text\n\t");

            for (auto const &zhChar : pText->m_text)
                message += zhChar.zhChar();

            message += "\\zhuyin{" + arg + "}";
            appendWarningMessage(status, message);
        }
    } else {
        status.status = eCommandStatus::Warning;
        status.message = "\\zhuyin{} must be specified immediately after text";
    }

    return status;
}
