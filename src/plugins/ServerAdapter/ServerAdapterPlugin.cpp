/*!
   \file ServerAdapterPlugin.cpp
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

#include "ServerAdapterPlugin.h"
#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace ServerAdapter {

/*! \namespace Plugins::ServerAdapter
    \brief Contains the ServerAdapterPlugin, which helps demonstrate the
           methodologies for creating a GUI plugin.
 */

/*! \class ServerAdapterPlugin
    \version 0.1.dev
    \brief This is an example of the methods for producing a GUI plugin.

    \par Depends on Plugins:
         OpenSpeedShop

    \todo Document this more explicitly as a developer example.
 */

ServerAdapterPlugin::ServerAdapterPlugin()
{
    m_Name = "ServerAdapter";
    m_Version = "0.1.dev";
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", "^0\\.1.*$") );
}

ServerAdapterPlugin::~ServerAdapterPlugin()
{
}

bool ServerAdapterPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(&m_ServerAdapter);
    } catch(...) {
        return false;
    }

    return true;
}

void ServerAdapterPlugin::shutdown()
{
}

QString ServerAdapterPlugin::name()
{
    return m_Name;
}

QString ServerAdapterPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> ServerAdapterPlugin::dependencies()
{
    return m_Dependencies;
}

} // namespace ServerAdapter
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::ServerAdapter::ServerAdapterPlugin)
