/*!
   \file SettingPage.h
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

#ifndef PLUGINSETTINGPAGE_H
#define PLUGINSETTINGPAGE_H

#include <QDialog>
#include <QTreeWidget>
#include <SettingManager/ISettingPage.h>
#include <SettingManager/SettingManager.h>
#include "PluginWrapper.h"

namespace Core {
namespace PluginManager {

namespace Ui {
    class PluginSettingPage;
}

class PluginSettingPage : public SettingManager::ISettingPage
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPage)

public:
    explicit PluginSettingPage(QList<PluginWrapper *> plugins, QWidget *parent = 0);
    explicit PluginSettingPage(QWidget *parent = 0);
    ~PluginSettingPage();

public slots:
    void apply();
    void reset();

protected:
    void changeEvent(QEvent *e);

    void readSettings();
    void writeSettings();

    void buildTree(QList<PluginWrapper *> plugins);
    QList<QTreeWidgetItem *> m_Plugins;

private:
    Ui::PluginSettingPage *ui;

};

}}
#endif // PLUGINSETTINGPAGE_H
