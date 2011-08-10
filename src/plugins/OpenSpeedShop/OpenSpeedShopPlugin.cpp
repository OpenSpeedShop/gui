/*!
   \file OpenSpeedShopPlugin.cpp
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2011 Argo Navis Technologies, LLC

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

#ifdef QT_DEBUG
#  include <QtDebug>
#endif

#include "OpenSpeedShopPlugin.h"

#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>
#include <ConnectionManager/ConnectionManager.h>
#include <ModelManager/ModelManager.h>
#include <ViewManager/ViewManager.h>

#include "AboutDialog.h"
#include "Settings/SettingPage.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \namespace Plugins::OpenSpeedShop
    \brief Contains the base plugin for the Open|SpeedShop set of plugins.
 */

/*! \class OpenSpeedShopPlugin
    \version 0.1.dev
    \brief The base plugin for the Open|SpeedShop set of plugins.

    \par Depends on Plugins:
         none

    \sa ConnectionManager, ViewManager
 */


OpenSpeedShopPlugin::OpenSpeedShopPlugin() :
    m_AboutPage(this)
{
    m_Name = "OpenSpeedShop";
    m_Version = "0.1.dev";

    // If we're in production mode, show the splash screen!
#ifndef QT_DEBUG
    AboutDialog::splash();
#endif
}

OpenSpeedShopPlugin::~OpenSpeedShopPlugin()
{
}

bool OpenSpeedShopPlugin::initialize(QStringList &args, QString *err)
{
    using namespace Core;

    try {

        readSettings();

        /*** We're a main plugin, so we need to make changes to the mainWindow,
             like the application icon and the title ***/
        MainWindow::MainWindow &mainWindow = MainWindow::MainWindow::instance();
        mainWindow.setWindowTitle(m_MainWidget.windowTitle());
        mainWindow.setWindowIcon(m_MainWidget.windowIcon());

        /*** Set our main widget in the main window ***/
        mainWindow.addCentralWidget(&m_MainWidget, 32, QString("O|SS"), m_MainWidget.windowIcon());

        /*** Register our menu structure ***/
        foreach(QAction *action, mainWindow.menuBar()->actions()) {
            if(action->text() == tr("Help")) {
                m_AboutPage.setText(tr("About Open|SpeedShop"));
                m_AboutPage.setToolTip(tr("Displays the Open|SpeedShop about dialog"));
                connect(&m_AboutPage, SIGNAL(triggered()), this, SLOT(aboutDialog()));
                action->menu()->addAction(&m_AboutPage);
            }
        }

        /*** Register any objects with the plugin manager ***/
        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);                         /* Register ourselves as an ISettingPageFactory */
        pluginManager.addObject(&m_MainWidget);

        // This should be fault tollerant, as it's not mandatory to function properly
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

        ViewManager &viewManager = ViewManager::instance();
        if(!viewManager.initialize(args, err)) { throw; }

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
    ViewManager::instance().shutdown();

    writeSettings();
}

void OpenSpeedShopPlugin::aboutDialog()
{
    try {
        AboutDialog about;
        about.exec();
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open about dialog: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open about dialog."), NotificationWidget::Critical);
    }
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
    settingManager.beginGroup("Plugins/OpenSpeedShop");


    settingManager.endGroup();
}

void OpenSpeedShopPlugin::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop");


    settingManager.endGroup();
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

Q_EXPORT_PLUGIN(Plugins::OpenSpeedShop::OpenSpeedShopPlugin)

