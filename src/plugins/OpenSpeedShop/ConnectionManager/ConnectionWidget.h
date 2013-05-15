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

#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <SettingManager/ISettingPage.h>

namespace Plugins {
namespace OpenSpeedShop {

class IConnection;
namespace Ui { class ConnectionWidget; }

class ConnectionWidget : public Core::SettingManager::ISettingPage
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPage)

public:
    explicit ConnectionWidget(QWidget *parent = 0);
    ~ConnectionWidget();
public slots:
    void apply();
    void reset();

protected:
    void initialize();
    void startTimeOut(int msec = 3500);
    void stopTimeOut();

protected slots:
    void connectionRegistered(IConnection *);
    void on_cmbConnectionType_currentIndexChanged(int index);

private:
    Ui::ConnectionWidget *ui;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // CONNECTIONWIDGET_H
