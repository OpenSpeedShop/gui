/*!
   \file ServerAdapterPlugin.h
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

#ifndef PLUGINS_SERVERADAPTER_SERVERADAPTERPLUGIN_H
#define PLUGINS_SERVERADAPTER_SERVERADAPTERPLUGIN_H



#include <PluginManager/IPlugin.h>

#include "ServerAdapter.h"

namespace Plugins {
namespace ServerAdapter {

class ServerAdapterPlugin : public QObject, public Core::PluginManager::IPlugin {
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.krellinst.oss.ServerAdapterPlugin")
#endif

    Q_INTERFACES(Core::PluginManager::IPlugin)

public:
    ServerAdapterPlugin();

    /* IPlugin Interface */
    ~ServerAdapterPlugin();
    bool initialize(QStringList &args, QString *err);
    void shutdown();
    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();

protected:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;
    ServerAdapter m_ServerAdapter;

};



} // namespace ServerAdapter
} // namespace Plugins

#endif // PLUGINS_SERVERADAPTER_SERVERADAPTERPLUGIN_H
