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

#include "ViewManager.h"

#include <PluginManager/PluginManager.h>

#include "IViewFactory.h"

namespace Plugins {
namespace OpenSpeedShop {

ViewManager &ViewManager::instance()
{
    static ViewManager instance;
    return instance;
}

ViewManager::ViewManager(QObject *parent) :
    QObject(parent)
{
}

bool ViewManager::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)

    try {

        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(this);

        /* Check the object pool for anything we should manage */
        foreach(QObject *object, pluginManager.allObjects()) { pluginObjectRegistered(object); }
        connect(&pluginManager, SIGNAL(objectAdded(QObject*)), this, SLOT(pluginObjectRegistered(QObject*)));
        connect(&pluginManager, SIGNAL(objectRemoving(QObject*)), this, SLOT(pluginObjectDeregistered(QObject*)));

    } catch(...) {
        if(err->isEmpty()) { err->append(QChar::LineSeparator); }
        err->append(tr("Could not initialize ViewManager"));
        return false;
    }

    return true;
}

void ViewManager::shutdown()
{
}

void ViewManager::pluginObjectRegistered(QObject *object)
{
    IViewFactory *viewFactory = qobject_cast<IViewFactory *>(object);
    if(viewFactory) registerViewFactory(viewFactory);
}

void ViewManager::pluginObjectDeregistered(QObject *object)
{
    IViewFactory *viewFactory = qobject_cast<IViewFactory *>(object);
    if(viewFactory) deregisterViewFactory(viewFactory);
}


void ViewManager::registerViewFactory(IViewFactory *viewFactory)
{
    if(!m_viewFactories.contains(viewFactory->viewName())) {
        m_viewFactories.insert(viewFactory->viewName(), viewFactory);
    }
}

void ViewManager::deregisterViewFactory(IViewFactory *viewFactory)
{
    if(m_viewFactories.contains(viewFactory->viewName())) {
        m_viewFactories.remove(viewFactory->viewName());
    }
}

QStringList ViewManager::viewNames(QAbstractItemModel *model)
{
    QStringList nameList;
    foreach(IViewFactory *viewPlugin, m_viewFactories.values()) {
        if(model) {
            if(viewPlugin->viewHandles(model)) {
                nameList.append(viewPlugin->viewName());
            }
        } else {
            nameList.append(viewPlugin->viewName());
        }
    }
    return nameList;
}

QAbstractItemView *ViewManager::viewWidget(QString name, QAbstractItemModel *model)
{
    if(name.isEmpty()) return NULL;

    IViewFactory *viewPlugin = m_viewFactories.value(name, NULL);
    return viewPlugin->viewWidget(model);
}



} // namespace OpenSpeedShop
} // namespace Plugins
