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

#include "SettingPageFactory.h"

namespace Plugins {
namespace OpenSpeedShop {

/*!
   \class SettingPageFactory
   \brief Handles the interaction between the SettingManager and the
          Open|SpeedShop SettingPage.

   This page factory produces the SettingPage for the Open|SpeedShop plugin,
   and is registered with the SettingManager in the Core set of libraries.

   \sa OpenSpeedShopPlugin, SettingPage, Core::SettingManager
 */

SettingPageFactory::SettingPageFactory(QObject *parent) :
    QObject(parent)
{
}

SettingPageFactory::~SettingPageFactory()
{
}

QIcon SettingPageFactory::icon()
{
    return QIcon(":/OpenSpeedShop/app.png");
}

QString SettingPageFactory::name()
{
    return tr("Open|SpeedShop");
}

int SettingPageFactory::priority()
{
    return 128;
}

Core::SettingManager::ISettingPage *SettingPageFactory::createPage()
{
    return new SettingPage();
}

}}
