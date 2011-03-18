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

#include "Settings.h"
#include "ui_Settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

QIcon Settings::icon()
{
    return this->windowIcon();
}

QString Settings::name()
{
    return this->windowTitle();
}

int Settings::priority()
{
    return 50;
}

QWidget *Settings::widget()
{
    return this;
}

void Settings::initialize()
{
    Core::SettingManager::SettingManager *settingManager =
             Core::SettingManager::SettingManager::instance();

    //TODO: Get settings
    Q_UNUSED(settingManager);
}

void Settings::apply()
{
    Core::SettingManager::SettingManager *settingManager =
             Core::SettingManager::SettingManager::instance();

    //TODO: Persist settings
    Q_UNUSED(settingManager);
}
