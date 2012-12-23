/*!
   \file ServerConnectionPage.h
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

#ifndef SERVERCONNECTIONPAGE_H
#define SERVERCONNECTIONPAGE_H

#include <QtCore>
#include <QtGui>

#include <OpenSpeedShop/ConnectionManager/IConnectionPage.h>

namespace Plugins {
namespace ServerConnection {

class ServerConnection;
namespace Ui { class ServerConnectionPage; }

class ServerConnectionPage : public Plugins::OpenSpeedShop::IConnectionPage
{
    Q_OBJECT
    Q_INTERFACES(Plugins::OpenSpeedShop::IConnectionPage)

public:
    explicit ServerConnectionPage(ServerConnection *parentConnection, QWidget *parent = 0);
    ~ServerConnectionPage();

public slots:
    void apply();
    void reset();

protected:
    ServerConnection *m_ParentConnection;

private:
    Ui::ServerConnectionPage *ui;
};


} // namespace ServerConnection
} // namespace Plugins

#endif // SERVERCONNECTIONPAGE_H
