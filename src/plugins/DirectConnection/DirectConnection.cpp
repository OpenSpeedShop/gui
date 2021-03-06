/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2015 Argo Navis Technologies, LLC

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

#include <SettingManager/SettingManager.h>

#include "DirectConnectionPage.h"
#include <string>


#ifdef DIRECTCONNECTION_DEBUG
#  include <QDebug>
#endif

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace DirectConnection {

/*! \class DirectConnection
    \brief Handles the interaction as a compiled connection library
    \sa DirectConnectionPage, ConnectionManager, ServerConnection
 */

DirectConnection::DirectConnection(QObject *parent) :
    IConnection(parent)
{
    m_State = State_Disconnected;
    m_Buffer.clear();

    readSettings();
}

DirectConnection::~DirectConnection()
{
    writeSettings();

    foreach(DirectThread *directThread, m_DirectThreads) {
        directThread->cancel();
        m_DirectThreads.removeAll(directThread);
    }
}

QString DirectConnection::name() const
{
    return tr("Direct Connection");
}

IConnectionPage *DirectConnection::page()
{
    return new DirectConnectionPage(this, NULL);
}

void DirectConnection::connectToServer()
{
    setState(State_Connecting);
    setState(State_Connected);
}

void DirectConnection::disconnectFromServer()
{
    setState(State_Disconnecting);
    setState(State_Disconnected);
}

void DirectConnection::abort()
{
    setState(State_Disconnecting);
    setState(State_Disconnected);
}

QString DirectConnection::errorMessage()
{
    return m_ErrorMessage;
}

void DirectConnection::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop/ConnectionManager");


    settingManager.endGroup();
}

void DirectConnection::readSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop/ConnectionManager");


    settingManager.endGroup();
}

IConnection::States DirectConnection::state()
{
    return m_State;
}

void DirectConnection::setState(States state)
{
    m_State = state;
    emit stateChanged();
}

void DirectConnection::send(QString command)
{
    if(!command.endsWith('\n'))
        command.append('\n');

#ifdef OSSCLI_BLOCKING
    DirectThread *directThread = new DirectThread(command, &m_DirectCLI, &m_CLIMutex, this);
#else
    DirectThread *directThread = new DirectThread(command, this);
#endif

    connect(directThread, SIGNAL(resultReady(QString)), this, SLOT(handleResult(QString)));
    connect(directThread, SIGNAL(finished()), directThread, SLOT(deleteLater()));
    m_DirectThreads.append(directThread);
    directThread->start();
}

void DirectConnection::handleResult(const QString &result)
{
    DirectThread *directThread = qobject_cast<DirectThread*>(sender());
    if(directThread) {
        m_DirectThreads.removeAll(directThread);
    }

    m_Buffer = result;
    emit readyReceive();
}

QString DirectConnection::receive()
{
    QString retval = m_Buffer;
    m_Buffer.clear();
    return retval;
}

void DirectConnection::setIsCurrent(const bool &current)
{
    if(IConnection::isCurrent() == current) {
        return;
    }

    IConnection::setIsCurrent(current);

    // We have to pretend to connect in order to get the ConnectionManager to generate an Adapter
    if(IConnection::isCurrent()) {
        connectToServer();
    }
}

} // namespace DirectConnection
} // namespace Plugins

