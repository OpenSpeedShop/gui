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

#include "SettingPage.h"
#include "ui_SettingPage.h"

#include <QWidget>

namespace Plugins {
namespace OpenSpeedShop {

SettingPage::SettingPage(QWidget *parent) :
    Core::SettingManager::ISettingPage(parent),
    ui(new Ui::SettingPage)
{
    ui->setupUi(this);
    initialize();
}

SettingPage::~SettingPage()
{
    delete ui;
}

void SettingPage::initialize()
{
    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugin");
    settingManager->beginGroup("OpenSpeedShop");

    ui->lblSetting1->setText( "Default Project Path" );
    ui->txtSetting1->setText( settingManager->value("ProjectPath", "C:/Qt/projects/").toString() );

    settingManager->endGroup();
    settingManager->endGroup();

}

void SettingPage::apply()
{
    // Persist changed settings to SettingManager
    Core::SettingManager::SettingManager *settingManager =
            Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugin");
    settingManager->beginGroup("OpenSpeedShop");

    settingManager->setValue( "ProjectPath", ui->txtSetting1->text() );

    settingManager->endGroup();
    settingManager->endGroup();
}

void SettingPage::reset()
{
    initialize();
}

}}
