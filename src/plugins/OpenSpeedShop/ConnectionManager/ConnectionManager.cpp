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
#include "ServerAdapter.h"
#include "ServerCommand.h"
#include "ConnectionWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \class ConnectionManager
    \brief Handles the interaction between the Open|SpeedShop plugin set, and
           the Open|SpeedShop server.
    \sa ConnectionManagerPlugin
 */

ConnectionManager *m_ConnectionManagerInstance;
ConnectionManager *ConnectionManager::instance()
{
    return m_ConnectionManagerInstance? m_ConnectionManagerInstance: m_ConnectionManagerInstance = new ConnectionManager();
}

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{
    m_CurrentServerAdapter = NULL;
    m_CurrentConnection = NULL;
    m_DockWidget = NULL;
}

ConnectionManager::~ConnectionManager()
{
    // We're responsible for disposing of the connections
    while(!m_Connections.isEmpty())
        delete(m_Connections.takeFirst());
}

/*! \fn ConnectionManager::initialize()
    \brief Called when the GUI plugins are initializing
 */
bool ConnectionManager::initialize()
{
    Core::MainWindow::MainWindow *mainWindow = Core::MainWindow::MainWindow::instance();

    m_DockWidget = new QDockWidget(mainWindow);
    m_DockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_DockWidget->setWidget(new ConnectionWidget(m_DockWidget));
    mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_DockWidget, Qt::Vertical);

    readSettings();

    return true;
}

/*! \fn ConnectionManager::shutdown()
    \brief Called when the system is shutting down
 */
void ConnectionManager::shutdown()
{
    writeSettings();

    /* self destruct */
    //FIXME: There should be some handling for the case where instance() is called again after this
    delete(m_ConnectionManagerInstance);
    m_ConnectionManagerInstance = NULL;
}

/*! \fn ConnectionManager::readSettings()
    \brief Reads settings for the ConnectionManager from the SettingManager
 */
void ConnectionManager::readSettings()
{
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins/OpenSpeedShop/ConnectionManager");

    m_DockWidget->widget()->restoreGeometry(settingManager->value("DockWidget/WidgetGeometry").toByteArray());

    settingManager->endGroup();
}

/*! \fn ConnectionManager::writeSettings()
    \brief Writes settings for the ConnectionManager to the SettingManager
 */
void ConnectionManager::writeSettings()
{
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins/OpenSpeedShop/ConnectionManager");

    settingManager->setValue("DockWidget/WidgetGeometry", m_DockWidget->widget()->saveGeometry());

    settingManager->endGroup();
}

/*! \fn ConnectionManager::registerConnection()
    \brief Registers a potential connection type, and allows it to be displayed to the user as an option
    \param connection The new connection
 */
void ConnectionManager::registerConnection(IConnection *connection)
{
    connection->setParent(this);
    m_Connections.append(connection);
    emit connectionRegistered(connection);
}

/*! \fn ConnectionManager::setCurrentConnection()
    \brief Sets the current connection to a new value
    \param connection The new current connection
 */
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

ServerAdapter *ConnectionManager::currentServerAdapter()
{
    return m_CurrentServerAdapter;
}


/*! \fn ConnectionManager::currentConnection()
    \brief returns the currently selected connection.
 */
IConnection *ConnectionManager::currentConnection()
{
    return m_CurrentConnection;
}

/*! \fn ConnectionManager::connectionStateChanged()
    \brief Deals with the current connection's state changes.
 */
void ConnectionManager::connectionStateChanged()
{
    switch(m_CurrentConnection->state()) {
    case IConnection::State_Connected:
        m_CurrentServerAdapter = new ServerAdapter(this);
        break;
    case IConnection::State_Connecting:
    case IConnection::State_Disconnecting:
    case IConnection::State_Disconnected:
    case IConnection::State_Error:
        if(m_CurrentServerAdapter) {
            delete m_CurrentServerAdapter;
            m_CurrentServerAdapter = NULL;
        }

        foreach(ServerCommand *serverCommand, m_ServerCommands) {
            serverCommand->setState(ServerCommand::State_Invalid);
        }
        m_ServerCommands.clear();

        break;
    }

}

/*! \fn ConnectionManager::connectionReadyRecieve()
    \brief Recieves responses from a connection and sets the command's response.
    When the command's response is set, the state is changed to ServerCommand::State_Response and the
    ServerCommand::readyResponse() signal is emitted.
    \sa ServerCommand, ServerCommand::readyResponse(), ServerCommand::State_Response
 */
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

/*! \fn ConnectionManager::sendCommand()
    \brief Sends a ServerCommand object to the currentConnection, if it's connected.
    If a command sends to fail this function will return false, and the command's status will be set to
    ServerCommand::State_Invalid.  If it is successful, the function will return true and the command's state will be set to
    ServerCommand::State_Sent.
    \param command the ServerCommand to send.
    \returns false if there is no connection or the current connection is not connected to a server.
 */
bool ConnectionManager::sendCommand(ServerCommand *command)
{
    if(!currentConnection())
        return false;

    if(currentConnection()->state() != IConnection::State_Connected) {
        command->setState(ServerCommand::State_Invalid);
        return false;
    }

    m_ServerCommands.append(command);
    currentConnection()->send(command->command().toString());

    command->setState(ServerCommand::State_Sent);
    return true;
}


} // namespace OpenSpeedShop
} // namespace Plugins
