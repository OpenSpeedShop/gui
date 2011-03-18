/*!
   \file PluginDialog.cpp
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

#include "PluginDialog.h"
#include "ui_PluginDialog.h"

namespace Core {
namespace PluginManager {

PluginDialog::PluginDialog(QList<PluginWrapper *> plugins, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginDialog)
{
    ui->setupUi(this);

    readSettings();

    buildTree(plugins);
}

PluginDialog::~PluginDialog()
{
    writeSettings();

    delete ui;
}

void PluginDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PluginDialog::readSettings()
{
    SettingManager::SettingManager *settings =
            SettingManager::SettingManager::instance();

    settings->beginGroup("PluginManager");
    settings->beginGroup("PluginDialog");

    //TODO: Restore tree state
    resize( settings->value("WindowSize", size()).toSize() );
    move( settings->value("WindowPosition", pos()).toPoint() );

    settings->endGroup();
    settings->endGroup();
}

void PluginDialog::writeSettings()
{
    SettingManager::SettingManager *settings =
            SettingManager::SettingManager::instance();

    settings->beginGroup("PluginManager");
    settings->beginGroup("PluginDialog");

    //TODO: Store tree state
    settings->setValue("WindowSize", size());
    settings->setValue("WindowPosition", pos());

    settings->endGroup();
    settings->endGroup();
}

void PluginDialog::buildTree(QList<PluginWrapper *> plugins)
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
