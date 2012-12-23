/*!
   \file TreeViewPlugin.h
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

#ifndef TREEVIEWPLUGIN_H
#define TREEVIEWPLUGIN_H

#include <QtCore>
#include <QtPlugin>

#include <PluginManager/IPlugin.h>
#include <OpenSpeedShop/ViewManager/IViewFactory.h>

namespace Plugins {
namespace TreeView {

class TreeViewPlugin :
        public QObject,
        public Core::PluginManager::IPlugin,
        public Plugins::OpenSpeedShop::IViewFactory
{

    Q_OBJECT
    Q_INTERFACES(Core::PluginManager::IPlugin)
    Q_INTERFACES(Plugins::OpenSpeedShop::IViewFactory)

public:
    TreeViewPlugin(QObject *parent = 0);

    /* IPlugin Interface */
    ~TreeViewPlugin();
    bool initialize(QStringList &args, QString *err);
    void shutdown();
    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();

    /* IViewPlugin interface */
    QString viewName();
    bool viewHandles(QAbstractItemModel *model);
    QAbstractItemView *viewWidget(QAbstractItemModel *model);

protected:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;

};

} // namespace TreeView
} // namespace Plugins

#endif // TREEVIEWPLUGIN_H
