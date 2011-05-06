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

#ifndef ICONNECTION_H
#define ICONNECTION_H

#include <QObject>
#include <QWidget>
#include <QMetaType>
#include "ConnectionManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

enum ConnectionStates {
    ConnectionState_Connecting,
    ConnectionState_Connected,
    ConnectionState_Disconnecting,
    ConnectionState_Disconnected,
    ConnectionState_Error
};

class CONNECTIONMANAGER_EXPORT IConnection : public QObject
{
    Q_OBJECT

public:
    explicit IConnection(QObject *parent = 0) : QObject(parent) {}
    virtual ~IConnection() {}

    virtual QWidget *page() = 0;
    virtual ConnectionStates state() = 0;
    virtual QString errorMessage() = 0;
    virtual void connectToServer() = 0;
    virtual void disconnectFromServer() = 0;
    virtual void abort() = 0;

    virtual void send(QString command) = 0;
    virtual QString receive() = 0;

signals:
    void stateChanged();
    void readyReceive();

};


} // namespace OpenSpeedShop
} // namespace Plugins


//! We do this so that we can use the pointer in a QVariant
Q_DECLARE_METATYPE(Plugins::OpenSpeedShop::IConnection *);

Q_DECLARE_INTERFACE(Plugins::OpenSpeedShop::IConnection, "org.openspeedshop.gui.IConnection/0.1");

#endif // ICONNECTION_H
