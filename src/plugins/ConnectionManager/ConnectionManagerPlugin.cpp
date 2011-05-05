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

#include "ConnectionManagerPlugin.h"
#include "ConnectionManager.h"

namespace Plugins {
namespace ConnectionManager {

/*! \namespace Plugins::ConnectionManager
    \brief Contains the base plugin for the Open|SpeedShop ConnectionManager
           set of plugins.
 */

/*! \class ConnectionManagerPlugin
    \version 0.1.dev
    \brief Main plugin which manages the loading and initialization of the
           ConnectionManager.

    \par Depends on Plugins:
         OpenSpeedShop

    \sa ConnectionManager
 */

ConnectionManagerPlugin::ConnectionManagerPlugin(QObject *parent) :
    QObject(parent)
{
    m_Name = "ConnectionManager";
    m_Version = "0.1.dev";
    m_Dependencies.append( Dependency("OpenSpeedShop", "^0\\.1.*$") );

    ConnectionManager::instance(); // Create the singleton instance
}

ConnectionManagerPlugin::~ConnectionManagerPlugin()
{
}

bool ConnectionManagerPlugin::initialize(QStringList &args, QString *err)
{
    ConnectionManager *connectionManager = ConnectionManager::instance();
    return connectionManager->initialize();
}

void ConnectionManagerPlugin::shutdown()
{
    ConnectionManager *connectionManager = ConnectionManager::instance();
    connectionManager->shutdown();
}

QString ConnectionManagerPlugin::name()
{
    return m_Name;
}

QString ConnectionManagerPlugin::version()
{
    return m_Version;
}

QList<Dependency> ConnectionManagerPlugin::dependencies()
{
    return m_Dependencies;
}

Q_EXPORT_PLUGIN(Plugins::ConnectionManager::ConnectionManagerPlugin)

} // namespace ConnectionManager
} // namespace Plugins
