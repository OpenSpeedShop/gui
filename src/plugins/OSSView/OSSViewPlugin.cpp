/*!
   \file OSSViewPlugin.cpp
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

#include "OSSViewPlugin.h"

#include <PluginManager/PluginManager.h>
#include <ViewManager/ViewManager.h>
#include <TableView/TableView.h>

#include "OSSDelegate.h"

namespace Plugins {
namespace OSSView {

/*! \namespace Plugins::OSSView
    \brief Contains the OSSViewPlugin.
 */

/*! \class OSSViewPlugin
    \version 0.3.dev
    \brief

    \par Depends on Plugins:
         OpenSpeedShop
         TableView

    \todo Document this more explicitly.
 */

OSSViewPlugin::OSSViewPlugin(QObject *parent) : QObject(parent)
{
    m_Name = "OSSView";
    m_Version = QString("%1.%2.%3").arg(VER_MAJ).arg(VER_MIN).arg(VER_PAT);
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", QString("^%1\\.%2.*$").arg(VER_MAJ).arg(VER_MIN)) );
    m_Dependencies.append( Core::PluginManager::Dependency("TableView", QString("^%1\\.%2.*$").arg(VER_MAJ).arg(VER_MIN)) );
}

OSSViewPlugin::~OSSViewPlugin()
{
}

bool OSSViewPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {

        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(this);                         /* Register ourselves as an IViewFactory */

        //! \todo remove the IViewFactory for the TableView (so we can completely replace it with ours)

    } catch(...) {
        return false;
    }

    return true;
}

void OSSViewPlugin::shutdown()
{
}

QString OSSViewPlugin::name()
{
    return m_Name;
}

QString OSSViewPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> OSSViewPlugin::dependencies()
{
    return m_Dependencies;
}

QString OSSViewPlugin::viewName()
{
    return "Open|SpeedShop View";
}

bool OSSViewPlugin::viewHandles(QAbstractItemModel *model)
{
    Q_UNUSED(model)

    /* We can handle pretty much anything, so there's no checking needed */
    return true;
}

QAbstractItemView *OSSViewPlugin::viewWidget(QAbstractItemModel *model)
{
    /* Check to see if we should even try to handle this model */
    if(!viewHandles(model)) {
        return NULL;
    }

    Plugins::TableView::TableView *tableView = new Plugins::TableView::TableView();

    QAbstractItemDelegate *oldDelegate = tableView->itemDelegate();
    tableView->setItemDelegate(new OSSDelegate(this));
    oldDelegate->deleteLater();

    tableView->setModel(model);
    return tableView;
}

} // namespace OSSView
} // namespace Plugins

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Plugins::OSSView::OSSViewPlugin)
#endif
