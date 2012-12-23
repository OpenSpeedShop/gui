/*!
   \file LineGraphViewPlugin.cpp
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

#include "LineGraphViewPlugin.h"

#include "LineGraphView.h"

namespace Plugins {
namespace LineGraphView {

/*! \namespace Plugins::LineGraphView
    \brief Contains the LineGraphViewPlugin.
 */

/*! \class LineGraphViewPlugin
    \version 0.3.dev
    \brief

    \par Depends on Plugins:
         OpenSpeedShop

    \todo Document this more explicitly.
 */

LineGraphViewPlugin::LineGraphViewPlugin(QObject *parent) : QObject(parent)
{
    m_Name = "LineGraphView";
    m_Version = QString("%1.%2.%3").arg(VER_MAJ).arg(VER_MIN).arg(VER_PAT);
    m_Dependencies.append( Core::PluginManager::Dependency("OpenSpeedShop", QString("^%1\\.%2.*$").arg(VER_MAJ).arg(VER_MIN)) );
}

LineGraphViewPlugin::~LineGraphViewPlugin()
{
}

bool LineGraphViewPlugin::initialize(QStringList &args, QString *err)
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

void LineGraphViewPlugin::shutdown()
{
}

QString LineGraphViewPlugin::name()
{
    return m_Name;
}

QString LineGraphViewPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> LineGraphViewPlugin::dependencies()
{
    return m_Dependencies;
}

QString LineGraphViewPlugin::viewName()
{
    return "Line Graph View";
}

bool LineGraphViewPlugin::viewHandles(QAbstractItemModel *model)
{
    Q_UNUSED(model)

    //TODO: Ensure that the model matches what we can handle

    return true;
}

QAbstractItemView *LineGraphViewPlugin::viewWidget(QAbstractItemModel *model)
{
    /* Check to see if we should even try to handle this model */
    if(!viewHandles(model)) return NULL;

    LineGraphView *view = new LineGraphView();
    view->setModel(model);
    return view;
}

} // namespace LineGraphView
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::LineGraphView::LineGraphViewPlugin)
