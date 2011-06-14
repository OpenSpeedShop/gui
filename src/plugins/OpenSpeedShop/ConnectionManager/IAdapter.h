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

#ifndef IADAPTER_H
#define IADAPTER_H

#include <QObject>
#include <QMetaType>
#include <QStringList>
#include <QUuid>
#include <QAbstractItemModel>
#include "ConnectionManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class CONNECTIONMANAGER_EXPORT IAdapter : public QObject
{
    Q_OBJECT
public:
    enum Modes {
        Mode_Offline,
        Mode_Online
    };

    struct Process {
        QString host;
        qint64 processId;
        qint64 threadId;
        qint64 rank;
        QString executable;
    };

    explicit IAdapter(QObject *parent = 0);

    virtual bool isCompatible(QString serverVersion) = 0;

    virtual QString waitVersion() = 0;
    virtual void waitExit() = 0;

    virtual QStringList waitDirStat(QString path) = 0;
    virtual QString waitCatFile(QString filePath) = 0;

    virtual QStringList waitOssHelp() = 0;
    virtual QStringList waitExperimentTypes() = 0;
    virtual QUuid waitRestore(QString filepath) = 0;
    virtual QList<qint64> waitListOpenExperiments() = 0;
    virtual QString waitExperimentType(QUuid experimentUid) = 0;
    virtual QString waitExperimentDatabase(QUuid experimentUid) = 0;
    virtual QStringList waitExperimentMetrics(QUuid experimentUid) = 0;
    virtual QList<qint64> waitExperimentRanks(QUuid experimentUid) = 0;
    virtual QString waitExperimentAppCommand(QUuid experimentUid) = 0;
    virtual QString waitExperimentExecutable(QUuid experimentUid) = 0;
    virtual QStringList waitExperimentHosts(QUuid experimentUid) = 0;
    virtual QList<qint64> waitExperimentPids(QUuid experimentUid) = 0;
    virtual QStringList waitExperimentSourceFiles(QUuid experimentUid) = 0;
    virtual QStringList waitExperimentObjectFiles(QUuid experimentUid) = 0;
    virtual QList<qint64> waitExperimentThreads(QUuid experimentUid) = 0;
    virtual QStringList waitExperimentViews(QString experimentType) = 0;
    virtual QMap<QString, QVariant> waitExperimentParameterValues(QUuid experimentUid) = 0;
    virtual QAbstractItemModel *waitExperimentView(QUuid experimentUid, QStringList modifiers, QStringList metrics, int count) = 0;
    virtual QList<Process> waitExperimentProcesses(QUuid experimentUid) = 0;
    virtual QMap<QString,QString> waitExperimentTypeModifiers(QString experimentType) = 0;
    virtual QMap<QString,QString> waitExperimentTypeMetrics(QString experimentType) = 0;

};

} // namespace OpenSpeedShop
} // namespace Plugins

//! We do this so that we can use the pointer in a QVariant
Q_DECLARE_METATYPE(Plugins::OpenSpeedShop::IAdapter *);

Q_DECLARE_INTERFACE(Plugins::OpenSpeedShop::IAdapter, "org.openspeedshop.gui.IAdapter/0.1");

#endif // IADAPTER_H
