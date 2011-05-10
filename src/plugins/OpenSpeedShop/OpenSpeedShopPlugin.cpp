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
  #include <QtDebug>
#endif

#include "OpenSpeedShopPlugin.h"
#include <PluginManager/PluginManager.h>

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


OpenSpeedShopPlugin::OpenSpeedShopPlugin()
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
    readSettings();

    /*** We're a main plugin, so we need to make changes to the mainWindow,
         like the application icon and the title ***/
    Core::MainWindow::MainWindow *mainWindow = Core::MainWindow::MainWindow::instance();
    mainWindow->setWindowIcon(QIcon(":/OpenSpeedShop/app.png"));
    mainWindow->setWindowTitle(QString("Open|SpeedShop%1").arg(QChar(0x2122))); //Trademark

    /*** Set our main widget in the main window ***/
    _mainWidget = new OpenSpeedShopWidget(mainWindow);
    mainWindow->setCentralWidget(_mainWidget);

    /*** Register the settings page ***/
    Core::SettingManager::SettingManager *settingManager = Core::SettingManager::SettingManager::instance();
    settingManager->registerPageFactory(new SettingPageFactory());

    /*** Register our menu structure ***/
    foreach(QAction *action, mainWindow->menuBar()->actions()) {
        if(action->text() == tr("Help")) {
            QAction *about = new QAction(tr("About Open|SpeedShop"), this);
            about->setToolTip(tr("Displays the Open|SpeedShop about dialog"));
            connect(about, SIGNAL(triggered()), this, SLOT(aboutDialog()));
            action->menu()->addAction(about);
        }
    }

    ConnectionManager::instance()->initialize();
    Core::PluginManager::PluginManager::instance()->addObject(ConnectionManager::instance());

    return true;
}

void OpenSpeedShopPlugin::shutdown()
{
    ConnectionManager::instance()->shutdown();

    writeSettings();
}

void OpenSpeedShopPlugin::aboutDialog()
{
    AboutDialog about;
    about.exec();
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
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins");
    settingManager->beginGroup("OpenSpeedShop");


    settingManager->endGroup();
    settingManager->endGroup();
}

void OpenSpeedShopPlugin::writeSettings()
{
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins");
    settingManager->beginGroup("OpenSpeedShop");


    settingManager->endGroup();
    settingManager->endGroup();
}


Q_EXPORT_PLUGIN(Plugins::OpenSpeedShop::OpenSpeedShopPlugin)

}}

