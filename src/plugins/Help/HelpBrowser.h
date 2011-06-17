#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QtHelp>

namespace Plugins {
namespace Help {

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit HelpBrowser(QHelpEngine *helpEngine, QWidget *parent = 0);
    virtual QVariant loadResource(int type, const QUrl &url);

private:
    QHelpEngine *m_HelpEngine;
};

} // namespace Help
} // namespace Plugins

#endif // HELPBROWSER_H
