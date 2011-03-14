/*!
   \file OpenSpeedShopPlugin.cpp
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

#ifdef QT_DEBUG
  #include <QtDebug>
#endif

#include "OpenSpeedShopPlugin.h"
#include <ActionManager/ActionManager.h>

namespace Plugins {
namespace OpenSpeedShop {

OpenSpeedShopPlugin::OpenSpeedShopPlugin()
{
    m_Name = "OpenSpeedShop";
    m_Version = "0.1.dev";
}

OpenSpeedShopPlugin::~OpenSpeedShopPlugin()
{
}

bool OpenSpeedShopPlugin::initialize(QStringList &args, QString *err)
{
    Core::ActionManager::ActionManager *actions =
            Core::ActionManager::ActionManager::instance();

    // Create and connect actions to local slots
    QAction *action = new QAction(tr("Load"), this);
    action->setStatusTip(tr("Load an existing data set"));
    connect(action, SIGNAL(triggered()), this, SLOT(load()));
    actions->registerMenuItem("File", action);

    return true;
}

void OpenSpeedShopPlugin::shutdown()
{
}

void OpenSpeedShopPlugin::load()
{
#ifdef QT_DEBUG
    qDebug() << __FILE__ << __LINE__ << "Plugins::OpenSpeedShop::OpenSpeedShopPlugin::load()";
#endif

}

QString OpenSpeedShopPlugin::name()
{
    return m_Name;
}

QString OpenSpeedShopPlugin::version()
{
    return m_Version;
}

QList<Dependency> OpenSpeedShopPlugin::dependencies()
{
    return m_Dependencies;
}

Q_EXPORT_PLUGIN(Plugins::OpenSpeedShop::OpenSpeedShopPlugin)

}}

