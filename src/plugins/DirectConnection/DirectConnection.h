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

// The Open|SpeedShop CLI is non-reentrant right now; enable the OSSCLI_BLOCKING define when this changes!
#define OSSCLI_BLOCKING

#include <OpenSpeedShop/ConnectionManager/IConnection.h>

#include <openspeedshop/Direct.hxx>

#include <QThread>

#ifdef OSSCLI_BLOCKING
#include <QMutex>
#endif

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace DirectConnection {

class DirectConnectionPage;

class DirectThread : public QThread
{
    Q_OBJECT

public:

#ifdef OSSCLI_BLOCKING
    DirectThread(const QString &command, Direct *direct, QMutex *mutex, QObject *parent = 0) :
        QThread(parent), m_DirectCLI(direct), m_Mutex(mutex), m_Command(command), m_Canceled(false) { }
#else
    DirectThread(const QString &command, QObject *parent = 0) :
        QThread(parent), m_Command(command), m_Canceled(false) { }
#endif

    QString command() const { return m_Command; }
    void setCommand(const QString &command) { m_Command = command; }

    void cancel() { m_Canceled = true; }

signals:
    void resultReady(const QString &result);

private:
    void run() {

#ifdef OSSCLI_BLOCKING
        m_Mutex->lock();
        if(m_Canceled) { m_Mutex->unlock(); return; }  // nevermind!
        QString result = QString(m_DirectCLI->execute(std::string(m_Command.toLocal8Bit())).c_str());
        m_Mutex->unlock();
#else
        if(m_Canceled) { return; }  // nevermind!
        QString result = QString(m_DirectCLI.execute(std::string(m_Command.toLocal8Bit())).c_str());
#endif

        emit resultReady(result);
    }

#ifdef OSSCLI_BLOCKING
    Direct *m_DirectCLI;
    QMutex *m_Mutex;
#else
    Direct m_DirectCLI;
#endif

    QString m_Command;
    bool m_Canceled;
};

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

protected slots:
    void handleResult(const QString &result);

private:
    IConnection::States m_State;
    QString m_ErrorMessage;

    QString m_Buffer;

#ifdef OSSCLI_BLOCKING
    Direct m_DirectCLI;
    QMutex m_CLIMutex;
#endif
    QList<DirectThread *> m_DirectThreads;

    friend class DirectConnectionPage;
};

} // namespace DirectConnection
} // namespace Plugins
#endif // PLUGINS_DIRECTCONNECTION_DIRECTCONNECTION_H
