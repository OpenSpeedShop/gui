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

#include "DataModel.h"

namespace Plugins {
namespace ViewManager {

DataModel::DataModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    //TODO: Import and create model data in memory
}

DataModel::~DataModel()
{
    delete m_RootDataItem;
}

/* QAbstractItemModel interface */
QModelIndex DataModel::index(int row, int column, const QModelIndex &parent) const
{
    // Double check that we actually contain this index
    if(!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    // Create an index based on the data
    DataItem *parentItem;
    if(!parent.isValid()) {
        parentItem = m_RootDataItem;
    } else {
        parentItem = static_cast<DataItem *>(parent.internalPointer());
    }

    DataItem *childItem = parentItem->child(row);
    if(childItem) {
        return createIndex(row, column, childItem);
    }

    // Return an empty index if there's no data for this request
    return QModelIndex();
}

QModelIndex DataModel::parent(const QModelIndex &child) const
{
    if(!child.isValid()) {
        return QModelIndex();
    }

    DataItem *childItem = static_cast<DataItem *>(child.internalPointer());

    DataItem *parentItem = childItem->parent();
    if(parentItem != m_RootDataItem) {
        return createIndex(parentItem->row(), 0, parentItem);
    }

    return QModelIndex();
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        return m_RootDataItem->childCount();
    }

    DataItem *parentItem = static_cast<DataItem *>(parent.internalPointer());
    return parentItem->childCount();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        return m_RootDataItem->columnCount();
    }

    DataItem *parentItem = static_cast<DataItem *>(parent.internalPointer());
    return parentItem->columnCount();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    DataItem *dataItem = static_cast<DataItem *>(index.internalPointer());

    if(role == Qt::DisplayRole) {
        return dataItem->columnData(index.column());
    } else if(role == Qt::DecorationRole) {
        //TODO: Create and associate icons with DataItem types

//        if(index.column() == 0) {
//            QString dataType = dataItem->columnType(index.column());
//            if(dataType.toLower() == "") {
//                return QVariant(QIcon(""));
//            } else if(dataType.toLower().startsWith("commandresult_")) {
//                if(dataType.toLower().endsWith("")) {
//                    return QVariant(QIcon(""));
//                } else if(dataType.toLower().endsWith("")) {
//                    return QVariant(QIcon(""));
//                }
//            }
//        }

    }

    return QVariant();
}

} // namespace ViewManager
} // namespace Plugins
