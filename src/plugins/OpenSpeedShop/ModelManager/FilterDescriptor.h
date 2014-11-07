/*!
   \file FilterDescriptor.h
   \author Dane Gardner <dane.gardner@gmail.com>

   \section LICENSE
   This file is part of the Parallel Tools GUI Framework (PTGF)
   Copyright (C) 2010-2014 Argo Navis Technologies, LLC

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
 */

#ifndef FILTERDESCRIPTOR_H
#define FILTERDESCRIPTOR_H

#include <QObject>
#include <QUuid>
#include <QStringList>

namespace Plugins {
namespace OpenSpeedShop {

class FilterDescriptor : public QObject
{
    Q_OBJECT
public:
    explicit FilterDescriptor(QObject *parent = 0);

    QByteArray getHash() const;

    QStringList filters() const;
    bool isEmpty() const;

    QStringList hosts() const;
    void clearHosts();
    void removeHost(const QString &host);
    void insertHost(const QString &host);

    QStringList threads() const;
    void clearThreads();
    void removeThread(const QString &thread);
    void insertThread(const QString &thread);

    QStringList ranks() const;
    void clearRanks();
    void removeRank(const QString &rank);
    void insertRank(const QString &rank);

signals:
    void idChanged();
    void hostsChanged();
    void threadsChanged();
    void ranksChanged();
    void dataChanged();

protected:
    QStringList m_Hosts;
    QStringList m_Threads;
    QStringList m_Ranks;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // FILTERDESCRIPTOR_H
