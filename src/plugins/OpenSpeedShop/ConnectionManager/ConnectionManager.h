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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QList>
#include <QUuid>
#include <MainWindow/MainWindow.h>
#include <MainWindow/NotificationWidget.h>
#include "ConnectionManagerLibrary.h"
#include "ServerAdapter.h"
#include "ConnectionManagerSettingPageFactory.h"
#include "IAdapter.h"

namespace Plugins {
namespace OpenSpeedShop {

class IConnection;
class ServerCommand;

class CONNECTIONMANAGER_EXPORT ConnectionManager : public QObject
{
    Q_OBJECT
public:
    static ConnectionManager *instance();
    bool initialize();
    void shutdown();

    void registerConnection(IConnection *connection);
    IConnection *currentConnection();

    void registerAdapter(IAdapter *adapter);
    IAdapter *currentAdapter();
    IAdapter *askAdapter();

    bool sendCommand(ServerCommand *command);

    void connectToServer();
    void disconnectFromServer();
    bool isConnected();
    bool askServerConnect();


signals:
    void connectionRegistered(IConnection *connection);
    void currentConnectionChanged();

    void adapterRegistered(IAdapter *adapter);
    void currentAdapterChanged();

protected:
    ConnectionManager(QObject *parent = 0);
    ~ConnectionManager();
    void readSettings();
    void writeSettings();

    void setCurrentConnection(IConnection *connection);
    void setCurrentAdapter(IAdapter *adapter);

    QList<IConnection *> m_Connections;
    IConnection *m_CurrentConnection;
    QList<IAdapter *> m_Adapters;
    IAdapter *m_CurrentAdapter;
    QList<ServerCommand *> m_ServerCommands;

    Core::MainWindow::NotificationWidget *m_notifyConnecting;

protected slots:
    void connectionReadyRecieve();
    void connectionStateChanged();
    void serverConnect();

private:
    QAction m_ServerConnect;
    ConnectionManagerSettingPageFactory m_ConnectionManagerSettingPageFactory;
    ServerAdapter m_ServerAdapter;

    friend class ConnectionWidget;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // CONNECTIONMANAGER_H
