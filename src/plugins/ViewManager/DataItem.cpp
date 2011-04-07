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

#include "DataItem.h"

namespace Plugins {
namespace ViewManager {

DataItem::DataItem(const QString &type, const QVariant &data, DataItem *parent)
{
    m_Parent = parent;
    m_Type = type;
    m_Data = data;
}
DataItem::~DataItem()
{
    qDeleteAll(m_Columns);
    qDeleteAll(m_Rows);
}

int DataItem::columnCount() const
{
    return m_Columns.count();
}
DataItem *DataItem::column(int column)
{
    if(column > 0)
        return m_Columns.at(--column);

    return this;
}
QString DataItem::columnType(int column) const
{
    if(column > 0)
        return m_Columns.at(--column)->columnType(0);

    return m_Type;
}
QVariant DataItem::columnData(int column) const
{
    if(column > 0)
        return m_Columns.at(--column)->columnData(0);

    return m_Data;
}

int DataItem::childCount() const
{
    return m_Rows.count();
}
DataItem *DataItem::child(int row)
{
    return m_Rows.at(row);
}

DataItem *DataItem::parent()
{
    return m_Parent;
}
int DataItem::row() const
{
    return m_Parent->m_Rows.indexOf( const_cast<DataItem *>(this) );
}

} // namespace ViewManager
} // namespace Plugins
