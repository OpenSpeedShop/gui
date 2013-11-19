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

#ifndef PLUGINS_DIRECTCONNECTION_DIRECTCONNECTION_H
#define PLUGINS_DIRECTCONNECTION_DIRECTCONNECTION_H

#include <OpenSpeedShop/ConnectionManager/IConnection.h>

#include <openspeedshop/Direct.h>

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace DirectConnection {

class DirectConnectionPage;

class DirectConnection : public IConnection
{
    Q_OBJECT
    Q_INTERFACES(Plugins::OpenSpeedShop::IConnection)

public:
    explicit DirectConnection(QObject *parent = 0);
    ~DirectConnection();

    QString name() const;
    IConnectionPage *page();
    IConnection::States state();
    QString errorMessage();
    void connectToServer();
    void disconnectFromServer();
    void abort();

    void send(QString command);
    QString receive();

    void setIsCurrent(const bool &current);

protected:
    void writeSettings();
    void readSettings();
    void setState(States state);

private:
    IConnection::States m_State;
    QString m_ErrorMessage;

    QString m_Buffer;
    Direct m_DirectCLI;

    friend class DirectConnectionPage;
};

} // namespace DirectConnection
} // namespace Plugins
#endif // PLUGINS_DIRECTCONNECTION_DIRECTCONNECTION_H
