#include "HelpWidget.h"

#include <QVBoxLayout>
#include <QDesktopServices>
#include <MainWindow/MainWindow.h>
#include "HelpBrowser.h"

namespace Plugins {
namespace Help {

HelpWidget::HelpWidget(QHelpEngine *helpEngine, QWidget *parent) :
    QSplitter(parent),
    m_Documents(this)
{
    m_HelpEngine = helpEngine;

    setWindowTitle("Help");
    setWindowIcon(QIcon(":/Help/windowIcon.svg"));

    initSideBar();

    initDocumentTabs();

    QList<int> sizes;
    sizes << 100 << 1000;
    setSizes(sizes);

    QDesktopServices::setUrlHandler(QLatin1String("qthelp"), this, "linkActivated");

    //FIXME: Open home URL (this is a temporary method)
    QDesktopServices::openUrl(QUrl("qthelp://org.openspeedshop.gui/openspeedshop-gui/index.html"));
}

void HelpWidget::initSideBar()
{
    QTabWidget *tabContents = new QTabWidget(this);
    tabContents->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    addWidget(tabContents);
    setCollapsible(0, false);

    QHelpContentWidget *helpContentWidget = m_HelpEngine->contentWidget();
    connect(helpContentWidget, SIGNAL(linkActivated(QUrl)), this, SLOT(linkActivated(QUrl)));
    tabContents->addTab(helpContentWidget, tr("Content"));

    QHelpIndexWidget *helpIndexWidget = m_HelpEngine->indexWidget();
    connect(helpIndexWidget, SIGNAL(linkActivated(QUrl,QString)), this, SLOT(linkActivated(QUrl, QString)));
    tabContents->addTab(helpIndexWidget, tr("Index"));

    QWidget *searchLayoutWidget = new QWidget(tabContents);
    tabContents->addTab(searchLayoutWidget, tr("Search"));
    QVBoxLayout *searchLayout = new QVBoxLayout(searchLayoutWidget);
    searchLayoutWidget->setLayout(searchLayout);

    QHelpSearchQueryWidget *queryWidget = m_HelpEngine->searchEngine()->queryWidget();
    connect(queryWidget , SIGNAL(search()), this, SLOT(search()));
    searchLayout->addWidget(queryWidget );

    QHelpSearchResultWidget *resultWidget = m_HelpEngine->searchEngine()->resultWidget();
    connect(resultWidget, SIGNAL(requestShowLink(QUrl)), this, SLOT(linkActivated(QUrl)));
    searchLayout->addWidget(resultWidget);
}

void HelpWidget::initDocumentTabs()
{
    m_Documents.setDocumentMode(true);
    m_Documents.setTabsClosable(true);
    m_Documents.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(&m_Documents, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(&m_Documents, SIGNAL(currentChanged(int)), this, SLOT(tabCurrentChanged(int)));
    addWidget(&m_Documents);
    setCollapsible(1, false);
}

void HelpWidget::linkActivated(QUrl url, QString string)
{
    Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
    mainWindow.setCurrentCentralWidget(this);

    HelpBrowser *browser = new HelpBrowser(m_HelpEngine, &m_Documents);
    browser->setSource(url);
    m_Documents.addTab(browser, browser->documentTitle());
    m_Documents.setCurrentWidget(browser);
}

void HelpWidget::search()
{
    /* I have no idea why this isn't automatically wired up, everything else is!?!? */
    QHelpSearchEngine *searchEngine = m_HelpEngine->searchEngine();
    searchEngine->search(searchEngine->queryWidget()->query());
}

void HelpWidget::tabCloseRequested(int index)
{
    QWidget *widget = m_Documents.widget(index);
    m_Documents.removeTab(index);
    widget->close();
    widget->deleteLater();
}

void HelpWidget::tabCurrentChanged(int index)
{
    disconnect(this, SLOT(browserBackwardAvailable(bool)));
    disconnect(this, SLOT(browserForwardAvailable(bool)));

    if(index < 0) {
        emit backwardAvailable(false);
        emit forwardAvailable(false);
        return;
    }

    QWidget *widget = m_Documents.widget(index);
    HelpBrowser *helpBrowser = qobject_cast<HelpBrowser *>(widget);
    if(helpBrowser) {
        connect(helpBrowser, SIGNAL(backwardAvailable(bool)), this, SLOT(browserBackwardAvailable(bool)));
        connect(helpBrowser, SIGNAL(forwardAvailable(bool)), this, SLOT(browserForwardAvailable(bool)));

        emit backwardAvailable(helpBrowser->isBackwardAvailable());
        emit forwardAvailable(helpBrowser->isForwardAvailable());
    }
}

bool HelpWidget::isBackwardAvailable()
{
    QWidget *widget = m_Documents.currentWidget();
    HelpBrowser *helpBrowser = qobject_cast<HelpBrowser *>(widget);
    if(helpBrowser) {
        return helpBrowser->isBackwardAvailable();
    }
    return false;
}

bool HelpWidget::isForwardAvailable()
{
    QWidget *widget = m_Documents.currentWidget();
    HelpBrowser *helpBrowser = qobject_cast<HelpBrowser *>(widget);
    if(helpBrowser) {
        return helpBrowser->isForwardAvailable();
    }
    return false;
}

void HelpWidget::backward()
{
    QWidget *widget = m_Documents.currentWidget();
    HelpBrowser *helpBrowser = qobject_cast<HelpBrowser *>(widget);
    if(helpBrowser) {
        helpBrowser->backward();
    }
}

void HelpWidget::forward()
{
    QWidget *widget = m_Documents.currentWidget();
    HelpBrowser *helpBrowser = qobject_cast<HelpBrowser *>(widget);
    if(helpBrowser) {
        helpBrowser->forward();
    }
}

void HelpWidget::browserBackwardAvailable(bool available)
{
    emit backwardAvailable(available);
}

void HelpWidget::browserForwardAvailable(bool available)
{
    emit forwardAvailable(available);
}

} // namespace Help
} // namespace Plugins
