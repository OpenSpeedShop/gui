#include "HelpBrowser.h"

#include <QToolBar>
#include <QAction>

namespace Plugins {
namespace Help {

HelpBrowser::HelpBrowser(QHelpEngine *helpEngine, QWidget *parent) :
    QTextBrowser(parent)
{
    m_HelpEngine = helpEngine;

    //! \todo Add Back/Forward and Home buttons

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(16,16));
    setViewportMargins(0, toolBar->height(), 0, 0);

    QAction *backward = toolBar->addAction(QIcon(":/Help/backward.svg"), "Back", this, SLOT(backward()));
    connect(this, SIGNAL(backwardAvailable(bool)), backward, SLOT(setEnabled(bool)));

    QAction *forward = toolBar->addAction(QIcon(":/Help/forward.svg"), "Forward", this, SLOT(forward()));
    connect(this, SIGNAL(forwardAvailable(bool)), forward, SLOT(setEnabled(bool)));

    QAction *home = toolBar->addAction(QIcon(":/Help/home.svg"), "Home", this, SLOT(home()));
    connect(this, SIGNAL(backwardAvailable(bool)), home, SLOT(setEnabled(bool)));
}

QVariant HelpBrowser::loadResource(int type, const QUrl &url)
{
    if (url.scheme() == "qthelp") {
        return QVariant(m_HelpEngine->fileData(url));
    }

    return QTextBrowser::loadResource(type, url);
}

} // namespace Help
} // namespace Plugins
