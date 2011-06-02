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

namespace Plugins {
namespace OpenSpeedShop {

ViewManager *m_ViewManagerInstance;
ViewManager *ViewManager::instance()
{
    return m_ViewManagerInstance? m_ViewManagerInstance: m_ViewManagerInstance = new ViewManager();
}

ViewManager::ViewManager(QObject *parent) :
    QObject(parent)
{
}

void ViewManager::initialize()
{
}

void ViewManager::shutdown()
{
    /* self destruct! */
    //FIXME: There should be some handling for the case where instance() is called again after this
    delete(m_ViewManagerInstance);
    m_ViewManagerInstance = NULL;
}

void ViewManager::registerView(IViewPlugin *viewPlugin)
{
    m_viewPlugins.insert(viewPlugin->viewName(), viewPlugin);
}

QStringList ViewManager::viewNames(QAbstractItemModel *model)
{
    QStringList nameList;
    foreach(IViewPlugin *viewPlugin, m_viewPlugins.values()) {
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

    IViewPlugin *viewPlugin = m_viewPlugins.value(name, NULL);
    return viewPlugin->viewWidget(model);
}



} // namespace OpenSpeedShop
} // namespace Plugins
