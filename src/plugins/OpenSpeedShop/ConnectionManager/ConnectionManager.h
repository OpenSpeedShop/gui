/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2013 Argo Navis Technologies, LLC

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

#include <QtCore>
#include <QtGui>

#include <SettingManager/ISettingPageFactory.h>

#include "ConnectionManagerLibrary.h"

namespace Core { namespace MainWindow { class NotificationWidget; } }

namespace Plugins {
namespace OpenSpeedShop {

class IAdapter;
class IConnection;
class ServerCommand;

class CONNECTIONMANAGER_EXPORT ConnectionManager :
        public QObject,
        public Core::SettingManager::ISettingPageFactory
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPageFactory)

public:
    static ConnectionManager &instance();
    bool initialize(QStringList &args, QString *err);
    void shutdown();

    IConnection *currentConnection();
    IAdapter *currentAdapter();
    IAdapter *askAdapter();

    bool sendCommand(ServerCommand *command);

    void connectToServer();
    void disconnectFromServer();
    bool isConnected();
    bool askServerConnect();

    /* BEGIN ISettingPageFactory */
    QIcon settingPageIcon();
    QString settingPageName();
    int settingPagePriority();
    Core::SettingManager::ISettingPage *createSettingPage();
    /* END ISettingPageFactory */

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

    void registerConnection(IConnection *connection);
    void setCurrentConnection(IConnection *connection);
    QList<IConnection *> m_Connections;
    IConnection *m_CurrentConnection;

    void registerAdapter(IAdapter *adapter);
    void setCurrentAdapter(IAdapter *adapter);
    QList<IAdapter *> m_Adapters;
    IAdapter *m_CurrentAdapter;

    QList<ServerCommand *> m_ServerCommands;
    Core::MainWindow::NotificationWidget *m_NotifyConnecting;

protected slots:
    void pluginObjectRegistered(QObject *);
    void pluginObjectDeregistered(QObject *);
    void connectionReadyRecieve();
    void connectionStateChanged();
    void serverConnect();

private:
    QAction *m_ServerConnect;
    QLabel *m_lblConnectionStatus;

    friend class ConnectionWidget;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // CONNECTIONMANAGER_H
