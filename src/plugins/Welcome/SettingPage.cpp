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

#include <SettingManager/SettingManager.h>
#include <PluginManager/PluginManager.h>
#include "WelcomeWidget.h"

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace Welcome {

SettingPage::SettingPage(QWidget *parent) :
    Core::SettingManager::ISettingPage(parent),
    ui(new Ui::SettingPage)
{
    ui->setupUi(this);

    using namespace Core;
    PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
    QList<WelcomeWidget *> welcomeWidgets = pluginManager.getObjects<WelcomeWidget>();
    if(welcomeWidgets.count() < 1) {
        throw tr("A Welcome Widget was not found in the object pool");
    } else if(welcomeWidgets.count() > 1) {
        throw tr("More than one Welcome Widget was found in the object pool");
    }

    m_WelcomeWidget = welcomeWidgets.at(0);
    ui->lstRss->addItems(m_WelcomeWidget->rssNewsFeeds());

    reset();
}

SettingPage::~SettingPage()
{
    delete ui;
}

void SettingPage::apply()
{
    // Persist changed settings to SettingManager
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/Welcome");

    settingManager.setValue("rssEnabled", ui->grpRss->isChecked());
    m_WelcomeWidget->setNewsTabVisible(ui->grpRss->isChecked());

    settingManager.endGroup();
}

void SettingPage::reset()
{
    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/Welcome");

    ui->grpRss->setChecked(settingManager.value("rssEnabled", true).toBool());

    settingManager.endGroup();
}

void SettingPage::on_btnRssAdd_clicked()
{

}

void SettingPage::on_btnRssRemove_clicked()
{

}


} // namespace Welcome
} // namespace Plugins
