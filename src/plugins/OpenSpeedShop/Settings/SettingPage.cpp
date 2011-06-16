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

namespace Plugins {
namespace OpenSpeedShop {

/*!
   \class SettingPage
   \brief Widget that handles settings for the Open|SpeedShop plugin.

   This page is produced by the SettingPageFactory, which is registered
   with the SettingManager in the Core set of libraries.

   \sa OpenSpeedShopPlugin, SettingPageFactory, Core::SettingManager
 */


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
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop");

    ui->txtDatabasePath->setText(settingManager.value("Experiment/defaultExperimentPath", QString(QLatin1Char('/'))).toString());
    ui->txtSourcePath->setText(settingManager.value("Experiment/defaultSourcePath", QString(QLatin1Char('/'))).toString());

    bool okay;
    ui->txtExperimentSampleRate->setValue(settingManager.value("Experiment/defaultSampleRate", 60).toInt(&okay));
    if(!okay) { ui->txtExperimentSampleRate->setValue(60); }

    ui->txtModelRowCount->setValue(settingManager.value("ModelManager/defaultRowCount", 1000).toInt(&okay));
    if(!okay) { ui->txtModelRowCount->setValue(1000); }

    settingManager.endGroup();
}

void SettingPage::apply()
{
    // Persist changed settings to SettingManager
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop");

    settingManager.setValue("Experiment/defaultExperimentPath", ui->txtDatabasePath->text());
    settingManager.setValue("Experiment/defaultSourcePath", ui->txtSourcePath->text());
    settingManager.setValue("Experiment/defaultSampleRate", ui->txtExperimentSampleRate->value());
    settingManager.setValue("ModelManager/defaultRowCount", ui->txtModelRowCount->value());

    settingManager.endGroup();
}

void SettingPage::reset()
{
    initialize();
}

}}
