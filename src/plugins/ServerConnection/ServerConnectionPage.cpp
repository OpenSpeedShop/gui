/*!
   \file ServerConnectionPage.cpp
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2015 Argo Navis Technologies, LLC

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

#include "ServerConnectionPage.h"
#include "ui_ServerConnectionPage.h"

#include <SettingManager/SettingManager.h>

#include "ServerConnection.h"

namespace Plugins {
namespace ServerConnection {

/*! \class ServerConnectionPage
    \brief Widget that displays connection data and settings to the user for
           the ServerConnection class.
    \sa ServerConnection, ConnectionWidget, DirectConnectionPage
 */

ServerConnectionPage::ServerConnectionPage(ServerConnection *parentConnection, QWidget *parent) :
    IConnectionPage(parent),
    ui(new Ui::ServerConnectionPage)
{
    ui->setupUi(this);

    m_ParentConnection = parentConnection;
    setWindowTitle(m_ParentConnection->name());
    reset();
}

ServerConnectionPage::~ServerConnectionPage()
{
    delete ui;
}

void ServerConnectionPage::apply()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop/ConnectionManager/ServerConnection");
    settingManager.setValue("HostName", ui->txtHostname->text());
    settingManager.setValue("Port", ui->txtPort->value());
    settingManager.endGroup();

    m_ParentConnection->readSettings();
}

void ServerConnectionPage::reset()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop/ConnectionManager/ServerConnection");
    ui->txtHostname->setText(settingManager.value("HostName", m_ParentConnection->m_HostName).toString());
    ui->txtPort->setValue(settingManager.value("Port", m_ParentConnection->m_Port).toInt());
    settingManager.endGroup();
}

} // namespace ServerConnection
} // namespace Plugins
