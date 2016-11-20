#include <QDebug>
#include <QApplication>
#include <QPdfWriter>
#include <QScopedPointer>

#include "ast.h"
#include "astprinter.h"
#include "page.h"
#include "pagebuilder.h"
#include "pagedebugoutput.h"
#include "pagelayout.h"
#include "pagerenderer.h"
#include "qzy2ast.h"
#include "zhchar.h"

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

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QScopedPointer<AstNode> xRootNode;
    xRootNode.reset(testParser("example.qzy"));
    testPageBuilder(xRootNode.data());
    return 0;
}
