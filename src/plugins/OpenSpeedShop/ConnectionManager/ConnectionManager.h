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
#include <QDockWidget>
#include <QUuid>
#include <QGridLayout>
#include "ConnectionManagerLibrary.h"

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
    bool sendCommand(ServerCommand *command);

signals:
    void connectionRegistered(IConnection *connection);
    void currentConnectionChanged();

protected:
    ConnectionManager(QObject *parent = 0);
    ~ConnectionManager();
    void readSettings();
    void writeSettings();
    void setCurrentConnection(IConnection *connection);

    QList<IConnection *> m_Connections;
    QList<ServerCommand *> m_ServerCommands;
    QDockWidget *m_DockWidget;
    IConnection *m_CurrentConnection;

protected slots:
    void connectionReadyRecieve();
    void connectionStateChanged();

    friend class ConnectionWidget;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // CONNECTIONMANAGER_H
