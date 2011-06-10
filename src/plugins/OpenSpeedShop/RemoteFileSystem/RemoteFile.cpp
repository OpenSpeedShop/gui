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

#include "RemoteFile.h"

RemoteFile::RemoteFile(QObject *parent) :
    QObject(parent)
{
}

void RemoteFile::copy(const RemoteFile &other)
{
    m_Name = other.m_Name;
    m_Path = other.m_Path;
    m_Type = other.m_Type;
    m_Permissions = other.m_Permissions;
    m_HardLinks = other.m_HardLinks;
    m_UserId = other.m_UserId;
    m_GroupId = other.m_GroupId;
    m_Size = other.m_Size;
    m_AccessTime = other.m_AccessTime;
    m_ModifiedTime = other.m_ModifiedTime;
    m_ChangedTime = other.m_ChangedTime;
    m_Fetched = other.m_Fetched;
    m_Valid = other.m_Valid;
}

