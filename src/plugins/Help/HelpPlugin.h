#ifndef HELPPLUGIN_H
#define HELPPLUGIN_H

#include <QtHelp>
#include <QAction>
#include <PluginManager/IPlugin.h>
#include "HelpWidget.h"

namespace Plugins {
namespace Help {

class HelpPlugin : public QObject, public Core::PluginManager::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(Core::PluginManager::IPlugin)

public:
    explicit HelpPlugin(QObject *parent = 0);
    ~HelpPlugin();

    bool initialize(QStringList &args, QString *err);
    void shutdown();

    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();

    HelpWidget *helpWidget();

private:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;

    QHelpEngine m_HelpEngine;
    HelpWidget m_HelpWidget;

    QAction m_HelpBrowserBack;
    QAction m_HelpBrowserForward;

};

} // namespace Help
} // namespace Plugins

#endif // HELPPLUGIN_H
