/*!
   \file DirectAdapterPlugin.cpp
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

#include "DirectAdapterPlugin.h"

#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>
#include <PluginManager/PluginManager.h>

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace DirectAdapter {

/*! \namespace Plugins::DirectAdapter
    \brief Contains the DirectAdapterPlugin.
 */

/*! \class DirectAdapterPlugin
    \version 0.3.dev
    \brief

    \par Depends on Plugins:
         OpenSpeedShop

    \todo Document this more explicitly.
 */

DirectAdapterPlugin::DirectAdapterPlugin()
{
    m_Name = "DirectAdapter";
    m_Version = QString("%1.%2.%3").arg(VER_MAJ).arg(VER_MIN).arg(VER_PAT);
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", QString("^%1\\.%2.*$").arg(VER_MAJ).arg(VER_MIN)) );
}

DirectAdapterPlugin::~DirectAdapterPlugin()
{
}

bool DirectAdapterPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(&m_DirectAdapter);
    } catch(...) {
        return false;
    }

    return true;
}

void DirectAdapterPlugin::shutdown()
{
}

QString DirectAdapterPlugin::name()
{
    return m_Name;
}

QString DirectAdapterPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> DirectAdapterPlugin::dependencies()
{
    return m_Dependencies;
}

} // namespace DirectAdapter
} // namespace Plugins

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Plugins::DirectAdapter::DirectAdapterPlugin)
#endif
