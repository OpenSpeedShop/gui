/*!
   \file ServerConnectionPlugin.h
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

#ifndef SERVERCONNECTIONPLUGIN_H
#define SERVERCONNECTIONPLUGIN_H

#include <QtCore>

#include <PluginManager/IPlugin.h>

#include "ServerConnection.h"

namespace Plugins {
namespace ServerConnection {

class ServerConnectionPlugin : public QObject, public Core::PluginManager::IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA("org.krellinst.oss.ServerConnectionPlugin")
#endif

    Q_INTERFACES(Core::PluginManager::IPlugin)

public:
    explicit ServerConnectionPlugin(QObject *parent = 0);
    ~ServerConnectionPlugin();

    bool initialize(QStringList &args, QString *err);
    void shutdown();

    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();

protected:
//    void readSettings();
//    void writeSettings();

private:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;
    ServerConnection m_ServerConnection;

};

} // namespace ServerConnection
} // namespace Plugins

#endif // SERVERCONNECTIONPLUGIN_H
