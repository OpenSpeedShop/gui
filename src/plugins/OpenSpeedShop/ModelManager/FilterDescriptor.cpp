/*!
   \file FilterDescriptor.cpp
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

#include "FilterDescriptor.h"
#include <QCryptographicHash>

namespace Plugins {
namespace OpenSpeedShop {

FilterDescriptor::FilterDescriptor(QObject *parent) :
    QObject(parent)
{
}

/*! returns a probably unique hash (md5) for current filter set.
 */
QByteArray FilterDescriptor::getHash() const
{
    QCryptographicHash md5(QCryptographicHash::Md5);

    if(!m_Hosts.isEmpty()) {
        QStringList tmp(m_Hosts);
        tmp.sort();
        md5.addData(tmp.join("").toAscii());
    }

    if(!m_Threads.isEmpty()) {
        QStringList tmp(m_Threads);
        tmp.sort();
        md5.addData(tmp.join("").toAscii());
    }

    if(!m_Ranks.isEmpty()) {
        QStringList tmp(m_Ranks);
        tmp.sort();
        md5.addData(tmp.join("").toAscii());
    }

    return md5.result();
}


//HACK: this is absolutely not the right way to do this. Commands should be assembled in the Adapter!
QStringList FilterDescriptor::filters() const
{
    QStringList retval;

    if(!m_Hosts.isEmpty()) {
        retval.append("-h " + m_Hosts.join(","));
    }

    if(!m_Threads.isEmpty()) {
        retval.append("-t " + m_Threads.join(","));
    }

    if(!m_Ranks.isEmpty()) {
        retval.append("-r " + m_Ranks.join(","));
    }

    return retval;
}


bool FilterDescriptor::isEmpty() const
{
    return m_Hosts.isEmpty() && m_Threads.isEmpty() && m_Ranks.isEmpty();
}


QStringList FilterDescriptor::hosts() const
{
    return m_Hosts;
}

void FilterDescriptor::clearHosts()
{
    m_Hosts.clear();
}

void FilterDescriptor::removeHost(const QString &host)
{
    m_Hosts.removeOne(host);

    emit hostsChanged();
    emit dataChanged();
}

void FilterDescriptor::insertHost(const QString &host)
{
    m_Hosts.append(host);

    emit hostsChanged();
    emit dataChanged();
}


QStringList FilterDescriptor::threads() const
{
    return m_Threads;
}

void FilterDescriptor::clearThreads()
{
    m_Threads.clear();
}

void FilterDescriptor::removeThread(const QString &thread)
{
    m_Threads.removeOne(thread);

    emit threadsChanged();
    emit dataChanged();
}

void FilterDescriptor::insertThread(const QString &thread)
{
    m_Threads.append(thread);

    emit threadsChanged();
    emit dataChanged();
}


QStringList FilterDescriptor::ranks() const
{
    return m_Ranks;
}

void FilterDescriptor::clearRanks()
{
    m_Ranks.clear();
}

void FilterDescriptor::removeRank(const QString &rank)
{
    m_Ranks.removeOne(rank);

    emit ranksChanged();
    emit dataChanged();
}

void FilterDescriptor::insertRank(const QString &rank)
{
    m_Ranks.append(rank);

    emit ranksChanged();
    emit dataChanged();
}



} // namespace OpenSpeedShop
} // namespace Plugins
