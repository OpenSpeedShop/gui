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

#ifndef WELCOMEPLUGIN_H
#define WELCOMEPLUGIN_H

#include <PluginManager/IPlugin.h>
#include <SettingManager/ISettingPageFactory.h>

namespace Plugins {
namespace Welcome {

class WelcomePlugin :
        public QObject,
        public Core::PluginManager::IPlugin,
        public Core::SettingManager::ISettingPageFactory
{
    Q_OBJECT
    Q_INTERFACES(Core::PluginManager::IPlugin)
    Q_INTERFACES(Core::SettingManager::ISettingPageFactory)

public:
    explicit WelcomePlugin(QObject *parent = 0);

    /* BEGIN IPlugin */
    bool initialize(QStringList &args, QString *err);
    void shutdown();
    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();
    /* END IPlugin */

    /* BEGIN ISettingPageFactory */
    QIcon settingPageIcon();
    QString settingPageName();
    int settingPagePriority();
    Core::SettingManager::ISettingPage *createSettingPage();
    /* END ISettingPageFactory */

private:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;

};

} // namespace Welcom
} // namespace Plugins

#endif // WELCOMEPLUGIN_H
