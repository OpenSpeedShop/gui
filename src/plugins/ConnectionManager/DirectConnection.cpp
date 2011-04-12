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

#include "DirectConnection.h"

namespace Plugins {
namespace ConnectionManager {

/*!
   \class DirectConnection
   \brief Handles the interaction with a standard, remote (or local) TCP
          connected server.
   \sa DirectConnectionPage, ConnectionManager
 */


DirectConnection::DirectConnection(QObject *parent) :
    IConnection(parent)
{
//    m_TcpSocket = new QTcpSocket(this);

//    connect(m_TcpSocket, SIGNAL(readyRead()),
//            this, SLOT(readReady()));
//    connect(m_TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
//            this, SLOT(error(QAbstractSocket::SocketError)));
}

DirectConnection::~DirectConnection()
{
}

QWidget *DirectConnection::page()
{
    return new DirectConnectionPage();
}

bool DirectConnection::connect()
{
    bool RetVal = false;

    emit connecting();

    m_TcpSocket->connectToHost("localhost", 2048);

    //! \todo Set up a threaded wait
    if( (RetVal = m_TcpSocket->waitForConnected(5000)) )
        emit connected();

    return RetVal;
}

bool DirectConnection::disconnect()
{
    bool RetVal = false;

    emit disconnecting();

    m_TcpSocket->disconnectFromHost();

    //! \todo Set up a threaded wait
    if( (RetVal = m_TcpSocket->waitForDisconnected(5000)) )
        emit disconnected();

    return RetVal;
}

void DirectConnection::readReady()
{
}

void DirectConnection::error(QAbstractSocket::SocketError)
{
}



} // namespace OpenSpeedShop
} // namespace Plugins
