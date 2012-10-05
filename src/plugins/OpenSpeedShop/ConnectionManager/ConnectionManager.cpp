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
#include <MainWindow/NotificationWidget.h>
#include <SettingManager/SettingManager.h>
#include <PluginManager/PluginManager.h>

#include "IConnection.h"
#include "ServerCommand.h"
#include "ConnectionWidget.h"
#include "IAdapter.h"

#ifdef CONNECTIONMANAGER_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

/*! \class ConnectionManager
    \brief Handles the interaction between the Open|SpeedShop plugin set, and
           the Open|SpeedShop server.
    \sa ConnectionManagerPlugin
 */

ConnectionManager &ConnectionManager::instance()
{
    static ConnectionManager instance;

    return instance;
}

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{
    m_CurrentConnection = NULL;
    m_CurrentAdapter = NULL;
    m_NotifyConnecting = NULL;
    m_lblConnectionStatus = NULL;
    m_ServerConnect =  NULL;
}

ConnectionManager::~ConnectionManager()
{
}

/*! \fn ConnectionManager::initialize()
    \brief Called when the GUI plugins are initializing
 */
bool ConnectionManager::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)

    using namespace Core;

    try {

        /*** Register our menu structure ***/
        MainWindow::MainWindow &mainWindow = MainWindow::MainWindow::instance();
        foreach(QAction *action, mainWindow.menuBar()->actions()) {
            if(action->text() == tr("Tools")) {
                m_ServerConnect = new QAction(tr("Connect to server"), this);
                m_ServerConnect->setToolTip(tr("Connects to an Open|SpeedShop server"));
                m_ServerConnect->setIcon(QIcon(":/OpenSpeedShop/app.png"));
                m_ServerConnect->setIconVisibleInMenu(true);
                m_ServerConnect->setVisible(false);
                m_ServerConnect->setProperty("oss_menuitem", QVariant(1));
                connect(m_ServerConnect, SIGNAL(triggered()), this, SLOT(serverConnect()));

                QAction *ossAction = NULL;
                foreach(QAction *subAction, action->menu()->actions()) {
                    if(subAction->property("oss_menuitem").isValid()) {
                        ossAction = subAction;
                        break;
                    }
                }

                if(ossAction) {
                    action->menu()->insertAction(ossAction, m_ServerConnect);
                } else {
                    if(action->menu()->actions().count() > 0) {
                        action->menu()->insertSeparator(action->menu()->actions().at(0))->setProperty("oss_menuitem", QVariant(1));
                        action->menu()->insertAction(action->menu()->actions().at(0), m_ServerConnect);
                    } else {
                        action->menu()->addAction(m_ServerConnect);
                        action->menu()->addSeparator()->setProperty("oss_menuitem", QVariant(1));
                    }
                }

            }
        }

        if(!m_lblConnectionStatus) {
            m_lblConnectionStatus = new QLabel();
            m_lblConnectionStatus->setFixedSize(16,16);
            m_lblConnectionStatus->setPixmap(QPixmap(":/OpenSpeedShop/ConnectionManager/disconnected.svg"));
            m_lblConnectionStatus->setToolTip(tr("Disconnected"));
            mainWindow.statusBar()->addWidget(m_lblConnectionStatus);
        }

        readSettings();

        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);

        /* Check the object pool for anything we should manage */
        foreach(QObject *object, pluginManager.allObjects()) { pluginObjectRegistered(object); }
        connect(&pluginManager, SIGNAL(objectAdded(QObject*)), this, SLOT(pluginObjectRegistered(QObject*)));
        connect(&pluginManager, SIGNAL(objectRemoving(QObject*)), this, SLOT(pluginObjectDeregistered(QObject*)));

    } catch(...) {
        if(err->isEmpty()) err->append("\n");
        err->append(tr("Could not initialize ConnectionManager"));
        return false;
    }

    return true;
}

/*! \fn ConnectionManager::shutdown()
    \brief Called when the system is shutting down
 */
void ConnectionManager::shutdown()
{
    /* Shut down any connections to the server that we may have */
    IConnection *connection = currentConnection();
    if(connection && connection->state() == IConnection::State_Connected) {
        disconnectFromServer();
    }

    writeSettings();
}

/*! \fn ConnectionManager::readSettings()
    \brief Reads settings for the ConnectionManager from the SettingManager
 */
void ConnectionManager::readSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/ConnectionManager");
    QString connectionName = settingManager.value("ConnectionName", QString()).toString();
    settingManager.endGroup();

    foreach(IConnection *connection, m_Connections) {
        if(connection->name().compare(connectionName, Qt::CaseInsensitive) == 0) {
            setCurrentConnection(connection);
            break;
        }
    }

    if(!currentConnection() && m_Connections.count() > 0) {
        setCurrentConnection(m_Connections.at(0));
    }

}

/*! \fn ConnectionManager::writeSettings()
    \brief Writes settings for the ConnectionManager to the SettingManager
 */
void ConnectionManager::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/ConnectionManager");

    settingManager.endGroup();
}


void ConnectionManager::pluginObjectRegistered(QObject *object)
{
    IConnection *connection = qobject_cast<IConnection *>(object);
    if(connection) registerConnection(connection);

    IAdapter *adapter = qobject_cast<IAdapter *>(object);
    if(adapter) registerAdapter(adapter);
}

void ConnectionManager::pluginObjectDeregistered(QObject *object)
{
    Q_UNUSED(object)
    throw tr("Plugin object deregistration in the ConnectionManager has not been implemented");
}



/*! \fn ConnectionManager::registerConnection()
    \brief Registers a potential connection type, and allows it to be displayed to the user as an option
    \param connection The new connection
 */
void ConnectionManager::registerConnection(IConnection *connection)
{
    m_Connections.append(connection);
    emit connectionRegistered(connection);

    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/ConnectionManager");
    QString connectionName = settingManager.value("ConnectionName", QString()).toString();
    settingManager.endGroup();

    if(connection->name().compare(connectionName, Qt::CaseInsensitive) == 0) {
        setCurrentConnection(connection);
    }

    if(!currentConnection()) {
        setCurrentConnection(connection);
    }
}

/*! \fn ConnectionManager::setCurrentConnection()
    \brief Sets the current connection to a new value
    \param connection The new current connection
 */
void ConnectionManager::setCurrentConnection(IConnection *connection)
{
    if(!m_Connections.contains(connection))
        throw tr("Cannot set current connection to a connection that is not registered");

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

/*! \fn ConnectionManager::currentConnection()
    \brief returns the currently selected connection.
 */
IConnection *ConnectionManager::currentConnection()
{
    return m_CurrentConnection;
}


void ConnectionManager::registerAdapter(IAdapter *adapter)
{
    m_Adapters.append(adapter);
    emit adapterRegistered(adapter);
}

IAdapter *ConnectionManager::currentAdapter()
{
    return m_CurrentAdapter;
}

void ConnectionManager::setCurrentAdapter(IAdapter *adapter)
{
    if(adapter && !m_Adapters.contains(adapter))
        throw tr("Cannot set current adapter to an adapter that is not registered");

    if(adapter == m_CurrentAdapter)
        return;

    m_CurrentAdapter = adapter;
    emit currentAdapterChanged();
}

IAdapter *ConnectionManager::askAdapter()
{
    ConnectionManager &connectionManager = ConnectionManager::instance();
    if(!connectionManager.isConnected()) {
        if(!connectionManager.askServerConnect()) {
            return NULL;
        }
    }

    return connectionManager.currentAdapter();
}


/*! \fn ConnectionManager::connectionStateChanged()
    \brief Deals with the current connection's state changes.
 */
void ConnectionManager::connectionStateChanged()
{
    static IConnection::States previousState = IConnection::State_Disconnected;

    if(m_CurrentConnection->state() == IConnection::State_Connected) {
        /* Get the version string from the server, so we can load the right adapter */
        ServerCommand serverCommand("version", "Server");
        sendCommand(&serverCommand);
        while(serverCommand.state() != ServerCommand::State_Response) QApplication::processEvents();
        QDomElement responseElement = serverCommand.response().firstChildElement("Response");
        if(responseElement.isNull()) throw tr("Unable to get version from server");
        QDomElement serverResponseElement = responseElement.firstChildElement("ServerResponse");
        if(serverResponseElement.isNull()) throw tr("Unable to get version from server");
        if(!serverResponseElement.hasAttribute("version")) throw tr("Unable to get version from server");
        QString version = serverResponseElement.attribute("version");

        /* Find compatible adapter based on the server version */
        foreach(IAdapter *adapter, m_Adapters) {
            if(adapter->isCompatible(version)) {
                setCurrentAdapter(adapter);
            }
        }
    } else {
        /* Reset the adapter */
        setCurrentAdapter(NULL);

        /* Invalidate any commands that were part of that adapter */
        foreach(ServerCommand *serverCommand, m_ServerCommands) {
            serverCommand->setState(ServerCommand::State_Invalid);
            m_ServerCommands.removeOne(serverCommand);
        }
    }

    /* Set up user interactions */
    using namespace Core::MainWindow;
    if(m_NotifyConnecting) {
        m_NotifyConnecting->close();
        m_NotifyConnecting->deleteLater();
        m_NotifyConnecting = NULL;
    }

    if(m_CurrentConnection->state() == IConnection::State_Connected) {
        m_ServerConnect->setText(tr("Disconnect from server"));
        m_ServerConnect->setEnabled(true);

        m_lblConnectionStatus->setPixmap(QPixmap(":/OpenSpeedShop/ConnectionManager/connected.svg"));
        m_lblConnectionStatus->setToolTip(tr("Connected"));

    } else if(m_CurrentConnection->state() == IConnection::State_Connecting) {
        m_ServerConnect->setText(tr("Connecting to server"));
        m_ServerConnect->setEnabled(false);

        m_NotifyConnecting = MainWindow::instance().notify(tr("Connecting to server"), NotificationWidget::Loading);
        m_lblConnectionStatus->setToolTip(tr("Connecting"));

    } else if(m_CurrentConnection->state() == IConnection::State_Disconnecting) {
        m_ServerConnect->setText(tr("Disconnecting from server"));
        m_ServerConnect->setEnabled(false);

        m_NotifyConnecting = MainWindow::instance().notify(tr("Disconnecting from server"), NotificationWidget::Loading);
        m_lblConnectionStatus->setToolTip(tr("Disconnecting"));

    } else if(m_CurrentConnection->state() == IConnection::State_Disconnected) {
        m_ServerConnect->setText(tr("Connect to server"));
        m_ServerConnect->setEnabled(true);

        m_lblConnectionStatus->setPixmap(QPixmap(":/OpenSpeedShop/ConnectionManager/disconnected.svg"));
        m_lblConnectionStatus->setToolTip(tr("Disconnected"));

    } else {
        m_ServerConnect->setText(tr("Connect to server"));
        m_ServerConnect->setEnabled(true);

        if(previousState == IConnection::State_Connected) {
            MainWindow::instance().notify(tr("Unexpected loss of connection to server: %1").arg(m_CurrentConnection->errorMessage()), NotificationWidget::Critical);
        } else if(previousState == IConnection::State_Connecting) {
            MainWindow::instance().notify(tr("Error occured while connecting to server: %1").arg(m_CurrentConnection->errorMessage()), NotificationWidget::Critical);
        } else if(previousState == IConnection::State_Disconnecting) {
            MainWindow::instance().notify(tr("Error occured while disconnecting from server: %1").arg(m_CurrentConnection->errorMessage()), NotificationWidget::Critical);
        }
    }

    previousState = m_CurrentConnection->state();
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
        throw tr("Caught signal from unexpected object");

    QDomDocument document("Response");
    document.setContent(connection->receive());

    QUuid commandID(document.firstChildElement("Response").attribute("commandID"));

    foreach(ServerCommand *serverCommand, m_ServerCommands) {
        if(serverCommand->m_id == commandID) {
            serverCommand->setResponse(document);
            m_ServerCommands.removeOne(serverCommand);
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

bool ConnectionManager::isConnected()
{
    return currentConnection()->state() == IConnection::State_Connected;
}

bool ConnectionManager::askServerConnect() {
    IConnection *connection = currentConnection();
    if(!connection) return false;

    QMessageBox msg(QMessageBox::Question, tr("Not connected"), tr("Connect to server?"), QMessageBox::Yes|QMessageBox::No);

    if(msg.exec() == QMessageBox::Yes) {
        connectToServer();
        while(connection->state() == IConnection::State_Connecting) {
            QApplication::processEvents();
        }
    }

    return isConnected();
}

void ConnectionManager::connectToServer()
{
    IConnection *connection = currentConnection();
    if(!connection) {
        throw tr("No connection available, while trying to connect to server");
    }

    if(connection->state() == IConnection::State_Disconnected) {
        connection->connectToServer();
    }
}

void ConnectionManager::disconnectFromServer()
{
    IConnection *connection = currentConnection();
    if(!connection) {
        throw tr("No connection available, while trying to disconnect from server");
    }

    if(connection->state() == IConnection::State_Connected) {
        connection->disconnectFromServer();
    }
}

void ConnectionManager::serverConnect()
{
    if(!isConnected()) {
        try {
            connectToServer();
        } catch(QString err) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Client error while attempting to connect to server: %1").arg(err), NotificationWidget::Critical);
        } catch(...) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Client error while attempting to connect to server."), NotificationWidget::Critical);
        }
    } else {
        try {
            disconnectFromServer();
        } catch(QString err) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Client error while attempting to disconnect from server: %1").arg(err), NotificationWidget::Critical);
        } catch(...) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Client error while attempting to disconnect from server."), NotificationWidget::Critical);
        }
    }
}

/* BEGIN ISettingPageFactory */
QIcon ConnectionManager::settingPageIcon()
{
    return QIcon(":/OpenSpeedShop/app.png");
}

QString ConnectionManager::settingPageName()
{
    return tr("Server Connections");
}

int ConnectionManager::settingPagePriority()
{
    return 128;
}

Core::SettingManager::ISettingPage *ConnectionManager::createSettingPage()
{
    return new ConnectionWidget();
}
/* END ISettingPageFactory */


} // namespace OpenSpeedShop
} // namespace Plugins
