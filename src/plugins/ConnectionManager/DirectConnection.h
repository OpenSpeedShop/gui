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

#ifndef DIRECTCONNECTION_H
#define DIRECTCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include "IConnection.h"

namespace Plugins {
namespace ConnectionManager {

class DirectConnectionPage;

class DirectConnection : public IConnection
{
    Q_OBJECT
public:
    explicit DirectConnection(QObject *parent = 0);
    ~DirectConnection();

    QWidget *page();
    ConnectionStates state();
    QString errorMessage();
    void connectToServer();
    void disconnectFromServer();
    void abort();

    void send(QString command);
    QString receive();

protected slots:
    void readReady();
    void error(QAbstractSocket::SocketError);
    void connected();
    void disconnected();

protected:
    void writeSettings();
    void readSettings();
    void setState(ConnectionStates state);

    QTcpSocket *m_TcpSocket;
    QTimer *m_TimeOut;
    QString m_HostName;
    int m_Port;
    ConnectionStates m_State;
    QString m_ErrorMessage;

    friend class DirectConnectionPage;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // DIRECTCONNECTION_H
