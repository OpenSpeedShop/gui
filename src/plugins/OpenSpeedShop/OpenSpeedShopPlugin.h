/*!
   \file OpenSpeedShopPlugin.h
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

#ifndef OPENSPEEDSHOPPLUGIN_H
#define OPENSPEEDSHOPPLUGIN_H

#include <QtCore>
#include <MainWindow/MainWindow.h>
#include <PluginManager/IPlugin.h>
#include <SettingManager/ISettingPageFactory.h>
#include "OpenSpeedShopWidget.h"

#include "RemoteFileSystem/RemoteFileEngineHandler.h"

namespace Plugins {
namespace OpenSpeedShop {

class OpenSpeedShopPlugin :
        public QObject,
        public Core::PluginManager::IPlugin,
        public Core::SettingManager::ISettingPageFactory
{
    Q_OBJECT
    Q_INTERFACES(Core::PluginManager::IPlugin)
    Q_INTERFACES(Core::SettingManager::ISettingPageFactory)

public:
    OpenSpeedShopPlugin();
    ~OpenSpeedShopPlugin();

    bool initialize(QStringList &args, QString *err);
    void shutdown();

    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();

    /* BEGIN ISettingPageFactory */
    QIcon settingPageIcon();
    QString settingPageName();
    int settingPagePriority();
    Core::SettingManager::ISettingPage *createSettingPage();
    /* END ISettingPageFactory */

public slots:
    void aboutDialog();

protected:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;

    OpenSpeedShopWidget m_MainWidget;

    QAction m_AboutPage;

    void readSettings();
    void writeSettings();

};



} // namespace OpenSpeedShop
} // namespace Plugins
#endif // OPENSPEEDSHOPPLUGIN_H
