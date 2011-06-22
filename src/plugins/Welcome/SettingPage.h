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

#ifndef WELCOMESETTINGPAGE_H
#define WELCOMESETTINGPAGE_H

#include <QWidget>
#include <SettingManager/ISettingPage.h>
#include <SettingManager/SettingManager.h>

namespace Plugins {
namespace Welcome {

class WelcomeWidget;
namespace Ui { class SettingPage; }

class SettingPage : public Core::SettingManager::ISettingPage
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPage)

public:
    SettingPage(QWidget *parent = 0);
    ~SettingPage();

public slots:
    void apply();
    void reset();

protected slots:
    void on_btnRssAdd_clicked();
    void on_btnRssRemove_clicked();

private:
    WelcomeWidget *m_WelcomeWidget;
    Ui::SettingPage *ui;
};


} // namespace Welcome
} // namespace Plugins
#endif // WELCOMESETTINGPAGE_H
