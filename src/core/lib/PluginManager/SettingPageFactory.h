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

#ifndef SETTINGPAGEFACTORY_H
#define SETTINGPAGEFACTORY_H

#include <QObject>
#include <SettingManager/ISettingPageFactory.h>
#include "SettingPage.h"

namespace Core {
namespace PluginManager {

class SettingPageFactory :
        public QObject,
        public SettingManager::ISettingPageFactory
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPageFactory)

public:
    SettingPageFactory(QObject *parent = 0);
    ~SettingPageFactory();

    /* ISettingPageFactory interface */
    QIcon icon();
    QString name();
    int priority();
    SettingManager::ISettingPage *createPage();
};

} // namespace PluginManager
} // namespace Core

#endif // SETTINGPAGEFACTORY_H
