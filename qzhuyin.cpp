#include <QFileInfo>
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

AstNode* parseSource(const Options &options)
{
    Qzy2Ast qzy2Ast(options.source);
    AstNode *pRootNode = qzy2Ast.parse();

    if (pRootNode && options.debug)
        AstPrinter astPrinter(*pRootNode);

    return pRootNode;
}

void renderPage(Page &page, QPdfWriter &pdfWriter)
{
    PageRenderer renderer(pdfWriter);
    renderer.render(page);
}

void pageBuilder(AstNode *pRootNode, const Options &options)
{
    QFileInfo sourceFileInfo(options.source);
    QPdfWriter pdfWriter(sourceFileInfo.baseName() + ".pdf");
    PageBuilder pageBuilder(pdfWriter, *pRootNode);
    QScopedPointer<Page> xPage;
    PageLayout pageLayout;

    do {
        xPage.reset(pageBuilder.nextPage());
        if (xPage) {
            if (options.debug)
                PageDebugOutput debug(*xPage);

            pageLayout.layout(*xPage);
            renderPage(*xPage, pdfWriter);
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
    xRootNode.reset(parseSource(options));
    pageBuilder(xRootNode.data(), options);
    return 0;
}
