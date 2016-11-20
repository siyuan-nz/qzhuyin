#include <QDebug>
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QPdfWriter>
#include <QScopedPointer>

#include <errno.h>

#include "ast.h"
#include "astprinter.h"
#include "page.h"
#include "pagebuilder.h"
#include "pagedebugoutput.h"
#include "pagelayout.h"
#include "pagerenderer.h"
#include "qzy2ast.h"
#include "zhchar.h"

struct Options {
    bool debug = false;
    QString source;
};

AstNode* testParser(const QString &fileName)
{
    Qzy2Ast qzy2Ast(fileName);
    QScopedPointer<AstNode> xRootNode(qzy2Ast.parse());
    AstPrinter astPrinter(*xRootNode);
    return xRootNode.take();
}

void testPageRenderer(Page &page, QPdfWriter &pdfWriter)
{
    PageRenderer renderer(pdfWriter);
    renderer.render(page);
}

void testPageBuilder(AstNode *pRootNode)
{
    QPdfWriter pdfWriter("output.pdf");
    PageBuilder pageBuilder(pdfWriter, *pRootNode);
    QScopedPointer<Page> xPage;
    PageLayout layout;

    do {
        xPage.reset(pageBuilder.nextPage());
        if (xPage) {
            PageDebugOutput debug(*xPage);
            layout.layout(*xPage);
            testPageRenderer(*xPage, pdfWriter);
        }
    } while (xPage);
}

void parseOptions(const QCoreApplication &app, Options &options)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Chinese typsetting tool");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file."));
    parser.addOptions({
        {{"d", "debug"},
            QCoreApplication::translate("main", "Debug output.")},
    });
    parser.process(app);

    options.debug = parser.isSet("debug");
    QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.isEmpty())
        parser.showHelp(EINVAL);
    else
        options.source = positionalArguments.first();
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("qzhuyin");
    QCoreApplication::setApplicationVersion("0.9");

    Options options;
    parseOptions(app, options);

    QScopedPointer<AstNode> xRootNode;
    xRootNode.reset(testParser(options.source));
    testPageBuilder(xRootNode.data());
    return 0;
}
