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
#include <MainWindow/MainWindow.h>
#include <SettingManager/SettingManager.h>
#include "IConnection.h"
#include "ConnectionWidget.h"
#include "DirectConnection.h"

namespace Plugins {
namespace OpenSpeedShop {

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    static ConnectionManager *instance();

    bool initialize();
    void shutdown();

    void registerConnection(IConnection *connection);

signals:
    void connectionRegistered(IConnection *);

public slots:

protected:
    ConnectionManager(QObject *parent = 0);
    ~ConnectionManager();
    QList<IConnection *> m_Connections;
    QDockWidget *m_DockWidget;

    void readSettings();
    void writeSettings();

    friend class ConnectionWidget;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // CONNECTIONMANAGER_H
