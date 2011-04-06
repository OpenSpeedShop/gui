/*!
   \file 
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

#include <QGridLayout>
#include "ConnectionManager.h"

namespace Plugins {
namespace ConnectionManager {


ConnectionManager *m_Instance;
ConnectionManager *ConnectionManager::instance()
{
    return m_Instance ? m_Instance : m_Instance = new ConnectionManager();
}

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{
}

ConnectionManager::~ConnectionManager()
{
    // We're responsible for disposing of the connections
    while(!m_Connections.isEmpty())
        delete(m_Connections.takeFirst());
}

bool ConnectionManager::initialize()
{
    Core::MainWindow::MainWindow *mainWindow =
            Core::MainWindow::MainWindow::instance();

    m_DockWidget = new QDockWidget(mainWindow);
    m_DockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_DockWidget->setWidget(new ConnectionWidget(m_DockWidget));
    mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_DockWidget, Qt::Vertical);

    readSettings();

    registerConnection(new DirectConnection());

    return true;
}

void ConnectionManager::shutdown()
{
    writeSettings();
}

void ConnectionManager::readSettings()
{
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins/OpenSpeedShop/ConnectionManager");

    m_DockWidget->widget()->restoreGeometry(settingManager->value("DockWidget/WidgetGeometry").toByteArray());

    settingManager->endGroup();
}

void ConnectionManager::writeSettings()
{
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins/OpenSpeedShop/ConnectionManager");

    settingManager->setValue("DockWidget/WidgetGeometry", m_DockWidget->widget()->saveGeometry());

    settingManager->endGroup();
}

void ConnectionManager::registerConnection(IConnection *connection)
{
    connection->setParent(this);
    m_Connections.append(connection);
    emit connectionRegistered(connection);
}


} // namespace OpenSpeedShop
} // namespace Plugins
