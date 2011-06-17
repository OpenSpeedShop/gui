#include "HelpBrowser.h"

namespace Plugins {
namespace Help {

HelpBrowser::HelpBrowser(QHelpEngine *helpEngine, QWidget *parent) :
    QTextBrowser(parent)
{
    m_HelpEngine = helpEngine;
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
