#include "HelpPlugin.h"

#include <QToolBar>
#include <PluginManager/PluginManager.h>
#include <MainWindow/MainWindow.h>
#include "Settings/SettingPage.h"

namespace Plugins {
namespace Help {

HelpPlugin::HelpPlugin(QObject *parent) :
    QObject(parent),
    m_HelpEngine(new QHelpEngine("OpenSpeedShop.qhc"))
{
    m_Name = "Help";
    m_Version = "0.1.dev";

    m_HelpEngine->setupData();
    m_HelpEngine->setAutoSaveFilter(false);

    QString helpFile = "openspeedshop-gui.qch";
    QString helpNamespace = "org.openspeedshop.gui";
    if(!m_HelpEngine->registeredDocumentations().contains(helpNamespace) &&
            !m_HelpEngine->registerDocumentation(helpFile)) {
        qWarning(QString("Registration of help file, \"%1\", failed: %2").arg(helpFile).arg(m_HelpEngine->error()).toLatin1());
    }
}

HelpPlugin::~HelpPlugin()
{
    if(m_HelpEngine) {
        /*! \bug There is a segfault bug(?) in Qt 4.7.2 that crashes at ~QHelpEngine */
//        delete m_HelpEngine;
        m_HelpEngine = 0;
    }
}


/* BEGIN IPlugin */
bool HelpPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        using namespace Core;

        HelpWidget *helpWidget = new HelpWidget(m_HelpEngine);

        MainWindow::MainWindow &mainWindow = MainWindow::MainWindow::instance();
        mainWindow.addCentralWidget(helpWidget, 256);

        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);
        pluginManager.addObject(helpWidget);

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
/* END IPlugin */


/* BEGIN ISettingPageFactory */
QIcon HelpPlugin::settingPageIcon()
{
    return QIcon(":/Help/windowIcon.svg");
}

QString HelpPlugin::settingPageName()
{
    return tr("Help");
}

int HelpPlugin::settingPagePriority()
{
    return 256;
}

Core::SettingManager::ISettingPage *HelpPlugin::createSettingPage()
{
    return new SettingPage();
}
/* END ISettingPageFactory */


} // namespace Help
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::Help::HelpPlugin)
