/*!
   \file PluginWrapper.h
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

#ifndef PLUGINWRAPPER_H
#define PLUGINWRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "IPlugin.h"

namespace Core {
namespace PluginManager {

class PluginWrapper : public QObject, public IPlugin {
    Q_OBJECT
    Q_INTERFACES(Core::PluginManager::IPlugin)

public:

    enum States {
        State_Error,
        State_Loaded,
        State_Initialized,
        State_Shutdown
    };

    PluginWrapper(IPlugin *plugin, QString filePath, QObject *parent = 0);
    ~PluginWrapper();

    States state();

    int priority();
    void setPriority(int priority);

    /* IPlugin interface */
    bool initialize(QStringList &args, QString *err);
    void shutdown();
    QString name();
    QString version();
    QList<Dependency> dependencies();

protected:
    QString m_FilePath;
    IPlugin *m_Plugin;
    States m_State;
    int m_Priority;
};


} // namespace PluginManager
} // namespace Core
#endif // PLUGINWRAPPER_H
