/*!
   \file ServerConnectionPlugin.cpp
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

#include "ServerConnectionPlugin.h"

#include <PluginManager/PluginManager.h>
#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace ServerConnection {

/*! \namespace Plugins::ServerConnection
    \brief Contains the ServerConnectionPlugin
 */

/*! \class ServerConnectionPlugin
    \version 0.3.dev
    \brief Main plugin which manages the loading and initialization of the
           ServerConnection.

    \par Depends on Plugins:
         OpenSpeedShop

    \sa ConnectionManager, DirectConnectionPlugin
 */


ServerConnectionPlugin::ServerConnectionPlugin(QObject *parent) :
    QObject(parent)
{
    m_Name = "ServerConnection";
    m_Version = QString("%1.%2.%3").arg(VER_MAJ).arg(VER_MIN).arg(VER_PAT);
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", QString("^%1\\.%2.*$").arg(VER_MAJ).arg(VER_MIN)) );
}

ServerConnectionPlugin::~ServerConnectionPlugin()
{
}

bool ServerConnectionPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(&m_ServerConnection);
    } catch(...) {
        return false;
    }

    return true;
}

void ServerConnectionPlugin::shutdown()
{
}

QString ServerConnectionPlugin::name()
{
    return m_Name;
}

QString ServerConnectionPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> ServerConnectionPlugin::dependencies()
{
    return m_Dependencies;
}

} // namespace ServerConnection
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::ServerConnection::ServerConnectionPlugin)
