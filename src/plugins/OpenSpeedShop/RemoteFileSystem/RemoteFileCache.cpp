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

#include "RemoteFileCache.h"
#include <QRegExp>
#include <QDomDocument>
#include <OpenSpeedShop/ConnectionManager/IAdapter.h>
#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>

namespace Plugins {
namespace OpenSpeedShop {

RemoteFileCache::RemoteFileCache(QObject *parent) :
    QObject(parent)
{
}

QMap<QString, RemoteFile> &RemoteFileCache::listing(const QString &path)
{
//    QRegExp protocol("^openss://", Qt::CaseInsensitive);
//    if(path.contains(protocol)) {
//        path = path.remove(protocol);
//    }

    RemoteFile file = m_Paths.value(path, RemoteFile());

    /* Pull the file from the cache if it's valid, and hasn't expired */
    if(file.isValid() && file.fetched().secsTo(QDateTime::currentDateTime()) <= 120) {
        return file.files();
    }


    /* Connect to the server, and get the listing */
    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    QDomDocument document("Listing");
    document.setContent(adapter->waitDirStat("/home/dane/"));

    QDomElement responseElement = document.firstChildElement("Response");
    if(responseElement.isNull()) throw tr("'Response' element doesn't exist, as expected.");

    QDomElement fileSystemElement = responseElement.firstChildElement("FileSystem");
    if(fileSystemElement.isNull()) throw tr("'FileSystem' element doesn't exist, as expected.");

    QDomElement directoryElement = fileSystemElement.firstChildElement("Dir");
    if(directoryElement.isNull()) {

        // Deal with other responses, like errors
        QDomElement exceptionElement = fileSystemElement.firstChildElement("Exception");
        if(!exceptionElement.isNull()) {
            throw tr("Exception returned from server: '%1'").arg(exceptionElement.text());
        }

        throw tr("'Dir' element doesn't exist, as expected. No error was given from the server.");
    }


    /* Set up the remote file object */
    static QRegExp getFileName("[/\\\\]([a-zA-Z0-9\\_\\-\\s]+)[/\\\\]*\\s*$");
    if(getFileName.exactMatch(path)) {
        file.setName(getFileName.cap(1));
    }
    file.files().clear();
    file.setPath(path);
    file.setFetched(QDateTime::currentDateTime());
    file.setValid();
    if(!m_Paths.keys().contains(path)) {
        m_Paths.insert(path, file);
    }


    /* Get each file, initialize it, and add it as a child */
    QDomElement fileElement = directoryElement.firstChildElement("File");
    while(!fileElement.isNull()) {
        RemoteFile child;

        child.setFetched(QDateTime::currentDateTime());
        child.setValid();

        child.setName(fileElement.attribute("name"));
        child.setPath(fileElement.attribute("path"));
        child.setType(fileElement.attribute("type"));
        child.setPermissions((quint16)fileElement.attribute("permissions").toULongLong());
        child.setHardLinks((quint16)fileElement.attribute("links", "0").toULongLong());
        child.setUserId((quint32)fileElement.attribute("userId", "0").toULongLong());
        child.setGroupId((quint32)fileElement.attribute("groupId", "0").toULongLong());
        child.setSize((quint64)fileElement.attribute("size", "0").toULongLong());
        child.setAccessTime(QDateTime::fromTime_t((uint)fileElement.attribute("accessTime", "0").toULongLong()));
        child.setModifiedTime(QDateTime::fromTime_t((uint)fileElement.attribute("modifiedTime", "0").toULongLong()));
        child.setCreatedTime(QDateTime::fromTime_t((uint)fileElement.attribute("createdTime", "0").toULongLong()));

        file.files().insert(child.name(), child);

        fileElement = fileElement.nextSiblingElement(fileElement.tagName());
    }


    return file.files();
}

RemoteFile RemoteFileCache::file(const QString &file, const QString &path)
{
    QMap<QString, RemoteFile> pathListing = listing(path);
    if(pathListing.contains(file)) {
        return pathListing[file];
    }

    return RemoteFile();
}



} // namespace OpenSpeedShop
} // namespace Plugins
