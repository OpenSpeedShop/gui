/*!
   \file TreeViewPlugin.cpp
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

#include "TreeViewPlugin.h"

#include "TreeView.h"

namespace Plugins {
namespace TreeView {

/*! \namespace Plugins::TreeView
    \brief Contains the TreeViewPlugin.
 */

/*! \class TreeViewPlugin
    \version 0.1.dev
    \brief

    \par Depends on Plugins:
         OpenSpeedShop

    \todo Document this more explicitly.
 */

TreeViewPlugin::TreeViewPlugin(QObject *parent) : QObject(parent)
{
    m_Name = "TreeView";
    m_Version = "0.1.dev";
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", "^0\\.1.*$") );
}

TreeViewPlugin::~TreeViewPlugin()
{
}

bool TreeViewPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {

        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(this);                         /* Register ourselves as an IViewFactory */

    } catch(...) {
        return false;
    }

    return true;
}

void TreeViewPlugin::shutdown()
{
}

QString TreeViewPlugin::name()
{
    return m_Name;
}

QString TreeViewPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> TreeViewPlugin::dependencies()
{
    return m_Dependencies;
}

QString TreeViewPlugin::viewName()
{
    return "Tree/Table View";
}

bool TreeViewPlugin::viewHandles(QAbstractItemModel *model)
{
    Q_UNUSED(model)

    /* We can handle pretty much anything, so there's no checking needed */
    return true;
}

QAbstractItemView *TreeViewPlugin::viewWidget(QAbstractItemModel *model)
{
    /* Check to see if we should even try to handle this model */
    if(!viewHandles(model)) return NULL;

    TreeView *treeView = new TreeView();
    treeView->setModel(model);
    return treeView;
}

} // namespace TreeView
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::TreeView::TreeViewPlugin)
