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

#ifndef REMOTEFILE_H
#define REMOTEFILE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QDateTime>

class RemoteFile : public QObject
{
    Q_OBJECT

public:
    RemoteFile(QObject *parent = 0);
    RemoteFile(const RemoteFile &other) { copy(other); }

    QMap<QString, RemoteFile> &files() { return m_Files; }

    QDateTime &fetched() { return m_Fetched; } const
    void setFetched(const QDateTime &fetched) { m_Fetched = fetched; } const

    bool &isValid() { return m_Valid; } const
    void setValid(bool valid = true) { m_Valid = valid; }


    QString &name() { return m_Name; } const
    void setName(const QString &name) { m_Name = name; }

    QString &path() { return m_Path; } const
    void setPath(const QString &path) { m_Path = path; }

    QString &type() { return m_Type; } const
    void setType(const QString &type) { m_Type = type; }

    quint16 &permissions() { return m_Permissions; } const
    void setPermissions(const quint16 &permissions) { m_Permissions = permissions; }

    quint16 &hardLinks() { return m_HardLinks; } const
    void setHardLinks(const quint16 &hardLinks) { m_HardLinks = hardLinks; }

    quint32 &userId() { return m_UserId; } const
    void setUserId(const quint32 &userId) { m_UserId = userId; }

    quint32 &groupId() { return m_GroupId; } const
    void setGroupId(const quint32 &groupId) { m_GroupId = groupId; }

    quint64 &size() { return m_Size; } const
    void setSize(const quint64 &size) { m_Size = size; }

    QDateTime &accessTime() { return m_AccessTime; } const
    void setAccessTime(const QDateTime &accessTime) { m_AccessTime = accessTime; }

    QDateTime &modifiedTime() { return m_ModifiedTime; } const
    void setModifiedTime(const QDateTime &modifiedTime) { m_ModifiedTime = modifiedTime; }

    QDateTime &createdTime() { return m_ChangedTime; } const
    void setCreatedTime(const QDateTime &changedTime) { m_ChangedTime = changedTime; }

    RemoteFile &operator =(const RemoteFile &other) { copy(other); return *this; }

protected:
    void copy(const RemoteFile &other);

private:
    QMap<QString, RemoteFile> m_Files;
    QDateTime m_Fetched;
    bool m_Valid;


    QString m_Name;
    QString m_Path;
    QString m_Type;
    quint16 m_Permissions;
    quint16 m_HardLinks;         //! Maximum 32k children.
    quint32 m_UserId;            //! Max recommended 65k, but the hard limit is 4M.
    quint32 m_GroupId;           //! Max recommended 65k, but the hard limit is 4M.
    quint64 m_Size;              //! Go for broke on this one.
    QDateTime m_AccessTime;
    QDateTime m_ModifiedTime;
    QDateTime m_ChangedTime;


};

#endif // REMOTEFILE_H
