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
#include "AboutDialog.h"

namespace Plugins {
namespace OpenSpeedShop {

OpenSpeedShopPlugin::OpenSpeedShopPlugin() :
    m_Name("OpenSpeedShop"),
    m_Version("0.1.dev")
{
    AboutDialog::splash();
}

OpenSpeedShopPlugin::~OpenSpeedShopPlugin()
{
}

bool OpenSpeedShopPlugin::initialize(QStringList &args, QString *err)
{
    readSettings();

    /*** We're a main plugin, so we need to make changes to the mainWindow,
         like the application icon and the title ***/
    Core::MainWindow::MainWindow *mainWindow =
            Core::MainWindow::MainWindow::instance();
    mainWindow->setWindowIcon(QIcon(":/OpenSpeedShop/app.png"));
    mainWindow->setWindowTitle(QString("Open|SpeedShop%1").arg(QChar(0x2122))); //Trademark

    //DEBUG: Just stick an empty widget into the central area for now
    mainWindow->setCentralWidget(new QWidget(mainWindow));

    /*** Register the settings page ***/
    Core::SettingManager::SettingManager *settingManager =
             Core::SettingManager::SettingManager::instance();
    settingManager->registerPageFactory(new SettingPageFactory());


    /*** Register our menu structure ***/
    Core::ActionManager::ActionManager *actionManager =
             Core::ActionManager::ActionManager::instance();

    // Create the actions that we'll use to interact with the user
    Core::ActionManager::MenuItem *load = new Core::ActionManager::MenuItem(this);
    load->action()->setText(tr("Load"));
    load->action()->setToolTip(tr("Load an existing data set"));
    connect(load->action(), SIGNAL(triggered()), this, SLOT(load()));

    // Build the menus and add the actions to them
    Core::ActionManager::MenuItem *fileMenu = new Core::ActionManager::MenuItem(this);
    fileMenu->action()->setText(tr("File"));
    fileMenu->addMenuItem(load);
    actionManager->registerMenuItem(fileMenu);

    ConnectionManager *connectionManager = ConnectionManager::instance();
    connectionManager->initialize();

    return true;
}

void OpenSpeedShopPlugin::shutdown()
{
    writeSettings();

    ConnectionManager *connectionManager = ConnectionManager::instance();
    connectionManager->shutdown();
}

void OpenSpeedShopPlugin::load()
{
#ifdef QT_DEBUG
    qDebug() << __FILE__ << __LINE__ << "Plugins::OpenSpeedShop::OpenSpeedShopPlugin::load()";
#endif

}

QString OpenSpeedShopPlugin::name()
{
    return m_Name;
}

QString OpenSpeedShopPlugin::version()
{
    return m_Version;
}

QList<Dependency> OpenSpeedShopPlugin::dependencies()
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

