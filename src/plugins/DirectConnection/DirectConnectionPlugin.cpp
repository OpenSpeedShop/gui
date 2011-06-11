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

#include "DirectConnectionPlugin.h"

#include <PluginManager/PluginManager.h>
#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace DirectConnection {

/*! \namespace Plugins::DirectConnection
    \brief Contains the DirectConnectionPlugin
 */

/*! \class DirectConnectionPlugin
    \version 0.1.dev
    \brief Main plugin which manages the loading and initialization of the
           DirectConnection.

    \par Depends on Plugins:
         OpenSpeedShop

    \sa ConnectionManager
 */


DirectConnectionPlugin::DirectConnectionPlugin(QObject *parent) :
    QObject(parent)
{
    m_Name = "DirectConnection";
    m_Version = "0.1.dev";
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", "^0\\.1.*$") );
}

DirectConnectionPlugin::~DirectConnectionPlugin()
{
}

bool DirectConnectionPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(&m_DirectConnection);
    } catch(...) {
        return false;
    }

    return true;
}

void DirectConnectionPlugin::shutdown()
{
}

QString DirectConnectionPlugin::name()
{
    return m_Name;
}

QString DirectConnectionPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> DirectConnectionPlugin::dependencies()
{
    return m_Dependencies;
}

} // namespace DirectConnection
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::DirectConnection::DirectConnectionPlugin)
