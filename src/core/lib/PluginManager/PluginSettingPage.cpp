/*!
   \file SettingPage.cpp
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

#include "PluginSettingPage.h"
#include "ui_PluginSettingPage.h"

namespace Core {
namespace PluginManager {

PluginSettingPage::PluginSettingPage(QList<PluginWrapper *> plugins, QWidget *parent) :
    SettingManager::ISettingPage(parent),
    ui(new Ui::PluginSettingPage)
{
    ui->setupUi(this);

    readSettings();

    /* This constructor is called from the PluginManager as a dialog. We need
       to disable editing of the settings from here */
    ui->txtPluginPath->setEnabled(false);
    ui->lblPluginPath->setEnabled(false);
    ui->btnPluginPath->setVisible(false);

    buildTree(plugins);
    reset();
}

PluginSettingPage::PluginSettingPage(QWidget *parent) :
    SettingManager::ISettingPage(parent),
    ui(new Ui::PluginSettingPage)
{
    ui->setupUi(this);

    readSettings();

    buildTree( PluginManager::PluginManager::instance()->m_Plugins );
    reset();
}

PluginSettingPage::~PluginSettingPage()
{
    writeSettings();

    delete ui;
}

void PluginSettingPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PluginSettingPage::apply()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("PluginManager");

    // Store user settable settings
    PluginManager::instance()->m_PluginPath = ui->txtPluginPath->text();

    /* We could let the PluginManager manage this setting (it's stored upon
       close)... but then if we did, if there was a crash, the setting would
       never be stored. So we shall for now, until there is a reason not to.*/
    settingManager->setValue("PluginPath", ui->txtPluginPath->text());

    settingManager->endGroup();
}

void PluginSettingPage::reset()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("PluginManager");

    // Restore user settable settings
    ui->txtPluginPath->setText(PluginManager::instance()->m_PluginPath);

    settingManager->endGroup();
}

void PluginSettingPage::readSettings()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("PluginManager");
    settingManager->beginGroup("SettingPage");

    //! todo Restore tree state
    resize( settingManager->value("WindowSize", size()).toSize() );
    move( settingManager->value("WindowPosition", pos()).toPoint() );

    settingManager->endGroup();
    settingManager->endGroup();
}

void PluginSettingPage::writeSettings()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("PluginManager");
    settingManager->beginGroup("SettingPage");

    //! \todo Store tree state
    settingManager->setValue("WindowSize", size());
    settingManager->setValue("WindowPosition", pos());

    settingManager->endGroup();
    settingManager->endGroup();
}

void PluginSettingPage::buildTree(QList<PluginWrapper *> plugins)
{
    QStringList headers;
    headers.append("Name");
    ui->treeWidget->setColumnWidth(0, 250);
    headers.append("Version");
    ui->treeWidget->setColumnWidth(1, 50);
    ui->treeWidget->setHeaderLabels(headers);
    ui->treeWidget->setHeaderHidden(false);

    // Plugins should already be in the proper order after initialization

    foreach(PluginWrapper *plugin, plugins) {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, plugin->name());
        item->setText(1, plugin->version());

        switch(plugin->status()) {
            case PluginStatus_Loaded:
                item->setToolTip(0, tr("Plugin loaded"));
                item->setIcon(0, QIcon(":/PluginManager/check.png"));
                break;
            case PluginStatus_Initialized:
                item->setToolTip(0, tr("Plugin initialized"));
                item->setIcon(0, QIcon(":/PluginManager/check.png"));
                break;
            case PluginStatus_Error:
                item->setToolTip(0, tr("Plugin failed"));
                item->setIcon(0, QIcon(":/PluginManager/fail.png"));
                break;
            case PluginStatus_Shutdown:
                item->setToolTip(0, tr("Plugin shutdown"));
                item->setIcon(0, QIcon(":/PluginManager/fail.png"));
                break;
            default:
                item->setToolTip(0, tr("Unknown status"));
                item->setIcon(0, QIcon(":/PluginManager/fail.png"));
                break;
        }

        m_Plugins.append(item);
    }
}

}}
