/*!
   \file OpenSpeedShopPlugin.cpp
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2015 Argo Navis Technologies, LLC

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 2.1 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

   \section DESCRIPTION

 */

#include "OpenSpeedShopPlugin.h"

#include <QFile>
#include <QApplication>
#include <QDebug>

#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>

#include <ConnectionManager/ConnectionManager.h>
#include <ModelManager/ModelManager.h>
#include <RemoteFileSystem/PathRewriter.h>
#include <Settings/SettingPage.h>
#include <Help/HelpManager.h>

#include "MainWindow.h"


namespace Plugins {
namespace OpenSpeedShop {

/*! \namespace Plugins::OpenSpeedShop
    \brief Contains the base plugin for the Open|SpeedShop set of plugins.
 */

/*! \class OpenSpeedShopPlugin
    \version 0.3.dev
    \brief The base plugin for the Open|SpeedShop set of plugins.

    \par Depends on Plugins:
         none

    \sa ConnectionManager
 */


OpenSpeedShopPlugin::OpenSpeedShopPlugin() :
    m_MainWindow(NULL)
{
    m_Name = "OpenSpeedShop";
    m_Version = QString("%1.%2.%3").arg(VER_MAJ).arg(VER_MIN).arg(VER_PAT);
}

OpenSpeedShopPlugin::~OpenSpeedShopPlugin()
{
}

bool OpenSpeedShopPlugin::initialize(QStringList &args, QString *err)
{
    using namespace Core;

    try {

        readSettings();

        if(!m_MainWindow) {
            m_MainWindow = new MainWindow(this);
        }

        /*** Register any objects with the plugin manager ***/
        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);                         /* Register ourselves as an ISettingPageFactory */
        pluginManager.addObject(m_MainWindow);
        pluginManager.addObject(m_MainWindow->mainWindowWidget());

        // This should be fault tolerant, as it's not mandatory to function properly
        try {
            m_WelcomeData.initialize();
            pluginManager.addObject(&m_WelcomeData);
        } catch(QString error) {
            if(!err) err = new QString();
            if(err->isEmpty()) err->append("\n");
            err->append(error);
        } catch(...) {
            if(!err) err = new QString();
            if(err->isEmpty()) err->append("\n");
            err->append(tr("Could not initialize Open|SpeedShop plugin's welcome page data"));
        }

        ConnectionManager &connectionManager = ConnectionManager::instance();
        if(!connectionManager.initialize(args, err)) { throw; }

        ModelManager &modelManager = ModelManager::instance();
        if(!modelManager.initialize(args, err)) { throw; }

        PathRewriter &pathRewriter = PathRewriter::instance();
        if(!pathRewriter.initialize(args, err)) { throw; }

        registerHelpFile();

    } catch(...) {
        if(!err) err = new QString();
        if(err->isEmpty()) err->append("\n");
        err->append(tr("Could not initialize Open|SpeedShop plugin"));
        return false;
    }

    return true;
}

void OpenSpeedShopPlugin::shutdown()
{
    ConnectionManager::instance().shutdown();
    ModelManager::instance().shutdown();
    PathRewriter::instance().shutdown();

    writeSettings();
}

QString OpenSpeedShopPlugin::name()
{
    return m_Name;
}

QString OpenSpeedShopPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> OpenSpeedShopPlugin::dependencies()
{
    return m_Dependencies;
}

void OpenSpeedShopPlugin::readSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop");


    settingManager.endGroup();
}

void OpenSpeedShopPlugin::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop");


    settingManager.endGroup();
}

void OpenSpeedShopPlugin::registerHelpFile()
{
    Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
    QList<Plugins::Help::HelpManager*> helpManagers = pluginManager.getObjects<Plugins::Help::HelpManager>();
    if(helpManagers.count() < 1) {
        return;
    }

    Plugins::Help::HelpManager *helpManager = helpManagers.first();

# ifdef Q_OS_WIN
    QString helpFile = QString("%1/oss/OpenSpeedShop.qch").arg(QApplication::instance()->applicationDirPath());
# else
    QString helpFile = QString("%1/../share/oss/OpenSpeedShop.qch").arg(QApplication::instance()->applicationDirPath());
# endif

    if(QFile::exists(helpFile) && !helpManager->registeredNamespaces().contains("org.krellinst.oss")) {
        if(!helpManager->registerHelpFile(helpFile)) {
            qWarning() << Q_FUNC_INFO << tr("Registration of help file, \"%1\", failed: %2").arg(helpFile).arg(helpManager->registrationError());
        }
    }
}



/* BEGIN ISettingPageFactory */
QIcon OpenSpeedShopPlugin::settingPageIcon()
{
    return QIcon(":/OpenSpeedShop/app.png");
}

QString OpenSpeedShopPlugin::settingPageName()
{
    return tr("Open|SpeedShop");
}

int OpenSpeedShopPlugin::settingPagePriority()
{
    return 128;
}

Core::SettingManager::ISettingPage *OpenSpeedShopPlugin::createSettingPage()
{
    return new SettingPage();
}
/* END ISettingPageFactory */


} // namespace OpenSpeedShop
} // namespace Plugins

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Plugins::OpenSpeedShop::OpenSpeedShopPlugin)
#endif
