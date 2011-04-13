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

#include "PluginSettingPageFactory.h"

namespace Core {
namespace PluginManager {

PluginSettingPageFactory::PluginSettingPageFactory(QObject *parent) :
    QObject(parent)
{
}

PluginSettingPageFactory::~PluginSettingPageFactory()
{
}

QIcon PluginSettingPageFactory::icon()
{
    return QIcon(":/PluginManager/plugin.png");
}

QString PluginSettingPageFactory::name()
{
    return tr("Plugins");
}

int PluginSettingPageFactory::priority()
{
    return 50;
}

Core::SettingManager::ISettingPage *PluginSettingPageFactory::createPage()
{
    return new PluginSettingPage();
}


} // namespace PluginManager
} // namespace Core
