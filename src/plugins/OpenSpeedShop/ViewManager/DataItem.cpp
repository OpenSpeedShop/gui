/*!
   \file DataItem.cpp
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
namespace OpenSpeedShop {

/*! \class DataItem
    \brief DataItem objects are used to represent heirarchical data returned
           from the Open|SpeedShop framework, and is generated from within the
           DataModel class.  There should really be no direct interaction with
           other classes.
 */

/*!
   \fn DataItem::DataItem()
   \brief Constructor
   \param type A QString describing the data type this object represents
   \param data A QVariant containing the data for this object
   \param parent The parent object. This can be NULL, if it's meant to be a
                 root object.
 */
DataItem::DataItem(const QString &type, const QVariant &data, DataItem *parent)
{
    m_Parent = parent;
    m_Type = type;
    m_Data = data;
}

/*!
   \fn DataItem::~DataItem()
   \brief Destructor
 */
DataItem::~DataItem()
{
    qDeleteAll(m_Columns);
    qDeleteAll(m_Rows);
}

/*!
   \fn DataItem::columnCount()
   \brief Returns the number of columns represented by this object.
   \returns An integer value representing the number of available columns.
   \sa columnCount(), column(), columnType(), columnData(), addColumn(), childCount()
 */
int DataItem::columnCount() const
{
    return m_Columns.count();
}

/*!
   \fn DataItem::column()
   \brief Returns the DataItem representing a specified column.
   \param column The desired DataItem column to be returned.
   \returns A DataItem representing a specified column.
   \sa columnCount(), columnType(), columnData(), addColumn(), child()
 */
DataItem *DataItem::column(int column)
{
    if(column > 0)
        return m_Columns.at(--column);

    return this;
}

/*!
   \fn Plugins::ViewManager::DataItem::columnType()
   \brief Returns string represting a specific column's type.
   \returns A QString represting a specific column's type.
   \sa columnCount(), column(), columnData(), addColumn()
 */
QString DataItem::columnType(int column) const
{
    if(column > 0)
        return m_Columns.at(--column)->columnType(0);

    return m_Type;
}

/*!
   \fn Plugins::ViewManager::DataItem::columnType()
   \brief Returns a variant object containing the data this object represents.
   \returns A QVariant object containing the data this object represents.
   \sa columnCount(), column(), columnType(), addColumn()
 */
QVariant DataItem::columnData(int column) const
{
    if(column > 0)
        return m_Columns.at(--column)->columnData(0);

    return m_Data;
}

/*!
   \fn Plugins::ViewManager::DataItem::addColumn()
   \brief Adds a column to this object.
   \param columnItem The DataItem to add as a column to this instance.
   \sa columnCount(), column(), columnType(), columnData()
 */
void DataItem::addColumn(DataItem *columnItem)
{
    m_Columns.append(columnItem);
}

/*!
   \fn Plugins::ViewManager::DataItem::childCount()
   \brief Returns the number of children parented by this object.
   \returns An integer value representing the number of available children.
   \sa child(), addChild(), columnCount()
 */
int DataItem::childCount() const
{
    return m_Rows.count();
}

/*!
   \fn Plugins::ViewManager::DataItem::child()
   \brief Returns the DataItem representing a specified row.
   \param row The desired DataItem row to be returned.
   \returns A DataItem representing the specified row.
   \sa childCount(), addChild(), column()
 */
DataItem *DataItem::child(int row)
{
    return m_Rows.at(row);
}

/*!
   \fn Plugins::ViewManager::DataItem::addChild()
   \brief Adds a row to this object.
   \param childItem The item to be added as a row to this instance.
   \sa childCount(), child(), addColumn()
 */
void DataItem::addChild(DataItem *childItem)
{
    m_Rows.append(childItem);
}

/*!
   \fn Plugins::ViewManager::DataItem::parent()
   \brief Returns the DataItem representing the parent of this instance.
   \returns A DataItem representing the parent of this instance.
   \sa child(), column()
 */
DataItem *DataItem::parent()
{
    return m_Parent;
}

/*!
   \fn Plugins::ViewManager::DataItem::row()
   \brief Returns the row number that this object represents within its parent
          DataItem.
   \returns An integer value representing the row number for this object
   \sa parent()
 */
int DataItem::row() const
{
    return m_Parent->m_Rows.indexOf( const_cast<DataItem *>(this) );
}



} // namespace OpenSpeedShop
} // namespace Plugins
