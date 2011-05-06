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

#include "ConnectionManager.h"

#include <MainWindow/MainWindow.h>
#include <SettingManager/SettingManager.h>

#include "IConnection.h"
#include "ServerCommand.h"
#include "ConnectionWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \class ConnectionManager
    \brief Handles the interaction between the Open|SpeedShop plugin set, and
           the Open|SpeedShop server.
    \sa ConnectionManagerPlugin
 */

ConnectionManager *m_Instance;
ConnectionManager *ConnectionManager::instance()
{
    return m_Instance ? m_Instance : m_Instance = new ConnectionManager();
}

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{
    m_CurrentConnection = NULL;
    m_DockWidget = NULL;
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

void ConnectionManager::setCurrentConnection(IConnection *connection)
{
    if(connection && !m_Connections.contains(connection))
        throw new QString("Cannot set current connection to a connection that is not registered");

    if(connection == m_CurrentConnection)
        return;

    if(m_CurrentConnection) {
        disconnect(m_CurrentConnection, SIGNAL(readyReceive()), this, SLOT(connectionReadyRecieve()));
        disconnect(m_CurrentConnection, SIGNAL(stateChanged()), this, SLOT(connectionStateChanged()));
    }

    m_CurrentConnection = connection;
    connect(m_CurrentConnection, SIGNAL(readyReceive()), this, SLOT(connectionReadyRecieve()));
    connect(m_CurrentConnection, SIGNAL(stateChanged()), this, SLOT(connectionStateChanged()));
    emit currentConnectionChanged();
}

IConnection *ConnectionManager::currentConnection()
{
    return m_CurrentConnection;
}

void ConnectionManager::connectionStateChanged()
{
    switch(m_CurrentConnection->state()) {
    case ConnectionState_Connected:
        break;
    case ConnectionState_Connecting:
    case ConnectionState_Disconnecting:
    case ConnectionState_Disconnected:
    case ConnectionState_Error:
        foreach(ServerCommand *serverCommand, m_ServerCommands) {
            serverCommand->setState(ServerCommandState_Invalid);
        }
        m_ServerCommands.clear();
        break;
    }

}

void ConnectionManager::connectionReadyRecieve()
{
    IConnection *connection = qobject_cast<IConnection *>(QObject::sender());

    if(!connection)
        throw new QString("Caught signal from unexpected object");

    QDomDocument document("Response");
    document.setContent(connection->receive());

    QUuid commandID(document.firstChildElement("Response").attribute("commandID"));

    foreach(ServerCommand *serverCommand, m_ServerCommands) {
        if(serverCommand->m_id == commandID) {
            serverCommand->setResponse(document);
            return;
        }
    }

    //! \todo Deal with not finding the ServerCommand
}

bool ConnectionManager::sendCommand(ServerCommand *command)
{
    if(currentConnection()->state() != ConnectionState_Connected) {
        command->setState(ServerCommandState_Invalid);
        return false;
    }

    m_ServerCommands.append(command);
    currentConnection()->send(command->command().toString());

    command->setState(ServerCommandState_Sent);
    return true;
}


} // namespace OpenSpeedShop
} // namespace Plugins
