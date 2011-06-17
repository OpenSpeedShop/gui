#include "HelpPlugin.h"

#include <QToolBar>
#include <PluginManager/PluginManager.h>
#include <MainWindow/MainWindow.h>

namespace Plugins {
namespace Help {

HelpPlugin::HelpPlugin(QObject *parent) :
    QObject(parent),
    m_HelpEngine("OpenSpeedShop.qhc"),
    m_HelpWidget(&m_HelpEngine),
    m_HelpBrowserBack(this),
    m_HelpBrowserForward(this)
{
    m_Name = "Help";
    m_Version = "0.1.dev";

    m_HelpEngine.setupData();

    QString helpFile = "openspeedshop-gui.qch";
    QString helpNamespace = "org.openspeedshop.gui";
    if(!m_HelpEngine.registeredDocumentations().contains(helpNamespace) &&
            !m_HelpEngine.registerDocumentation(helpFile)) {
        qWarning(QString("Registration of help file, \"%1\", failed: %2").arg(helpFile).arg(m_HelpEngine.error()).toLatin1());
    }
}

HelpPlugin::~HelpPlugin()
{
}

bool HelpPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        using namespace Core;


        MainWindow::MainWindow &mainWindow = MainWindow::MainWindow::instance();
        QToolBar *toolBar = mainWindow.addToolBar(tr("Help"));

        m_HelpBrowserBack.setText("Back");
        m_HelpBrowserBack.setShortcut(QKeySequence::Back);
        m_HelpBrowserBack.setDisabled(true);
        toolBar->addAction(&m_HelpBrowserBack);

        m_HelpBrowserForward.setText("Forward");
        m_HelpBrowserForward.setShortcut(QKeySequence::Forward);
        m_HelpBrowserForward.setDisabled(true);
        toolBar->addAction(&m_HelpBrowserForward);


        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);
        pluginManager.addObject(&m_HelpWidget);

    } catch(...) {
        return false;
    }

    return true;
}

void HelpPlugin::shutdown()
{
}

QString HelpPlugin::name()
{
    return m_Name;
}

QString HelpPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> HelpPlugin::dependencies()
{
    return m_Dependencies;
}

HelpWidget *HelpPlugin::helpWidget()
{
    return &m_HelpWidget;
}

} // namespace Help
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::Help::HelpPlugin)
