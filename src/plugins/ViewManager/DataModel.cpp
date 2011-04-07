/*!
   \file DataModel.cpp
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

/*!
   \class Plugins::ViewManager::DataModel
   \brief A DataModel is used as the interface between a data view and the set
          of data it is meant to display to the user.  This class takes care of
          the generation of the underlying data from an XML source, and the
          interaction with data views.
 */


/*!
   \fn Plugins::ViewManager::DataModel::DataModel()
   \brief Constructs a DataModel with the given parent
 */
DataModel::DataModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    //TODO: Import and create model data in memory
}

/*!
   \fn Plugins::ViewManager::DataModel::~DataModel()
   \brief Destroys the DataModel
 */
DataModel::~DataModel()
{
    if(m_RootDataItem) {
        delete m_RootDataItem;
        m_RootDataItem = NULL;
    }
}

/*! \brief Builds a data model out of DataItems from the provided XML string.
    \param xml A QString containing the XML document object model that
               describes this DataModel. */
void DataModel::loadData(QString xml)
{
    QDomDocument doc;
    doc.setContent(xml);

    QDomElement rootElement = doc.documentElement();
    if(rootElement.tagName().toLower() != "response") {
        //TODO: Deal with other responses, like errors
        throw new QString("Root item was not a response tag");
    }

    if(m_RootDataItem) {
        delete m_RootDataItem;
        m_RootDataItem = NULL;
    }

    m_RootDataItem = createDataItem(rootElement, NULL);
}

/*! \brief Creates a DataItem from an XML element, including it's children.
           Columns are created from attribute data, whereas children are
           created from child elements.
    \param element A QDomElement describing the returned DataItem and it's
                   children
    \param parent A pointer to the parent DataItem for which this item will be
                  a child.
    \returns A pointer to the created DataItem */
DataItem *DataModel::createDataItem(QDomElement element, DataItem *parent)
{
    QString type = element.tagName();
    QVariant data = element.text();
    DataItem *dataItem = new DataItem(type, data, parent);

    QDomNodeList childNodes = element.childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            DataItem *childItem = createDataItem(childElement, dataItem);
            dataItem->addChild(childItem);
        } else if(childNode.isAttr()) {
            QDomAttr attribute = childNode.toAttr();
            QString columnType = attribute.name();
            QVariant columnData = attribute.value();
            DataItem *column = new DataItem(columnType, columnData, parent);
            dataItem->addColumn(column);
        }
    }

    return dataItem;
}

/*! \brief Generates an XML document object model from this DataModel's data set.
    \returns An XML document in flat text for easy file saving. */
QString DataModel::saveData() const
{
    //TODO:
    return QString();
}


/* QAbstractItemModel interface */
/*! \brief Returns the index of the item in the model specified by the given
           row, column and parent index.

    \reimp Reimplemented from QAbstractItemModel.
    \sa createIndex() */
QModelIndex DataModel::index(int row, int column, const QModelIndex &parent) const
{
    /*! \note When reimplementing this function in a subclass, call
              createIndex() to generate model indexes that other components
              can use to refer to items in your model. */

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

/*! \brief Returns the parent of the model item with the given index. If the
           item has no parent, an invalid QModelIndex is returned.

           A common convention used in models that expose tree data structures
           is that only items in the first column have children. For that
           case, when reimplementing this function in a subclass the column of
           the returned QModelIndex would be 0.

    \reimp Reimplemented from QAbstractItemModel.
    \sa createIndex() */
QModelIndex DataModel::parent(const QModelIndex &child) const
{
    /*! \note When reimplementing this function in a subclass, be careful to
              avoid calling QModelIndex member functions, such as
              QModelIndex::parent(), since indexes belonging to your model
              will simply call your implementation, leading to infinite
              recursion. */

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

/*! \brief Returns the number of rows under the given parent. When the parent
           is valid it means that rowCount is returning the number of children
           of parent.

    \reimp Reimplemented from QAbstractItemModel.
    \sa columnCount() */
int DataModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        return m_RootDataItem->childCount();
    }

    DataItem *parentItem = static_cast<DataItem *>(parent.internalPointer());
    return parentItem->childCount();
}

/*! \brief Returns the number of columns for the children of the given parent.

    \reimp Reimplemented from QAbstractItemModel.
    \sa rowCount() */
int DataModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        return m_RootDataItem->columnCount();
    }

    DataItem *parentItem = static_cast<DataItem *>(parent.internalPointer());
    return parentItem->columnCount();
}

/*! \brief Returns the data stored under the given role for the item referred
           to by the index.

    \reimp Reimplemented from QAbstractItemModel.
    \sa Qt::ItemDataRole, setData(), and headerData() */
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
