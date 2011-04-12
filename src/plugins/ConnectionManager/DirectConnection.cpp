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
#include <QCoreApplication>

namespace Plugins {
namespace ConnectionManager {

/*! \class DirectConnection
    \brief Handles the interaction with a standard, remote (or local) TCP
           connected server.
    \sa DirectConnectionPage, ConnectionManager
 */


DirectConnection::DirectConnection(QObject *parent) :
    IConnection(parent)
{
    m_TcpSocket = new QTcpSocket(this);
    connect(m_TcpSocket, SIGNAL(readyRead()),
            this, SLOT(readReady()));
    connect(m_TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(m_TcpSocket, SIGNAL(connected()),
            this, SLOT(connected()));
    connect(m_TcpSocket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));
}

DirectConnection::~DirectConnection()
{
}

QWidget *DirectConnection::page()
{
    return new DirectConnectionPage();
}

bool DirectConnection::connectToServer()
{
    bool RetVal = false;
    emit connectingToServer();
    m_TcpSocket->connectToHost("localhost", 2048);
    return RetVal;
}

bool DirectConnection::disconnectFromServer()
{
    bool RetVal = false;
    emit disconnectingFromServer();
    m_TcpSocket->disconnectFromHost();
    return RetVal;
}

void DirectConnection::readReady()
{
}

void DirectConnection::error(QAbstractSocket::SocketError error)
{
    QString errorMsg;

    switch(error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorMsg = tr("The connection was refused by the peer (or timed out).");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorMsg = tr("The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.");
    case QAbstractSocket::HostNotFoundError:
        errorMsg = tr("The host address was not found.");
        break;
    case QAbstractSocket::SocketAccessError:
        errorMsg = tr("The socket operation failed because the application lacked the required privileges.");
        break;
    case QAbstractSocket::SocketResourceError:
        errorMsg = tr("The local system ran out of resources (e.g., too many sockets).");
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorMsg = tr("The socket operation timed out.");
        break;
    case QAbstractSocket::DatagramTooLargeError:
        errorMsg = tr("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).");
        break;
    case QAbstractSocket::NetworkError:
        errorMsg = tr("An error occurred with the network (e.g., the network cable was accidentally plugged out).");
        break;
    case QAbstractSocket::AddressInUseError:
        errorMsg = tr("The address specified to QUdpSocket::bind() is already in use and was set to be exclusive.");
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        errorMsg = tr("The address specified to QUdpSocket::bind() does not belong to the host.");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        errorMsg = tr("The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        errorMsg = tr("The socket is using a proxy, and the proxy requires authentication.");
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorMsg = tr("The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)");
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        errorMsg = tr("Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).");
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        errorMsg = tr("Could not contact the proxy server because the connection to that server was denied");
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        errorMsg = tr("The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)");
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        errorMsg = tr("The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.");
        break;
    case QAbstractSocket::ProxyNotFoundError:
        errorMsg = tr("The proxy address set with setProxy() (or the application proxy) was not found.");
        break;
    case QAbstractSocket::ProxyProtocolError:
        errorMsg = tr("The connection negotiation with the proxy server because the response from the proxy server could not be understood.");
        break;
    case QAbstractSocket::UnknownSocketError:
    default:
        errorMsg = tr("Unknown socket error");
        break;
    }

    emit connectionError(errorMsg);
}

void DirectConnection::connected()
{
    emit connectedToServer();
}

void DirectConnection::disconnected()
{
    emit disconnectedFromServer();
}


} // namespace OpenSpeedShop
} // namespace Plugins
