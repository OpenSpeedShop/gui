/*!
   \file
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

#include "DirectConnectionPage.h"
#include "ui_DirectConnectionPage.h"

#include <SettingManager/SettingManager.h>

#include "DirectConnection.h"

namespace Plugins {
namespace DirectConnection {

/*! \class DirectConnectionPage
    \brief Widget that displays connection data and settings to the user for
           the DirectConnection class.
    \sa DirectConnection, ConnectionWidget, ServerConnectionPage
 */

DirectConnectionPage::DirectConnectionPage(DirectConnection *parentConnection, QWidget *parent) :
    IConnectionPage(parent),
    ui(new Ui::DirectConnectionPage)
{
    ui->setupUi(this);

    m_ParentConnection = parentConnection;
    setWindowTitle(m_ParentConnection->name());
    reset();
}

DirectConnectionPage::~DirectConnectionPage()
{
    delete ui;
}

void DirectConnectionPage::apply()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/ConnectionManager/DirectConnection");


    settingManager.endGroup();

    m_ParentConnection->readSettings();
}

void DirectConnectionPage::reset()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/ConnectionManager/DirectConnection");


    settingManager.endGroup();
}

} // namespace DirectConnection
} // namespace Plugins
