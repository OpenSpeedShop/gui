/*!
   \file DataModel.cpp
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2015 Argo Navis Technologies, LLC

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

#include <QStringList>
#include <QFileInfo>
#include <QDebug>

namespace {
    QString truncatePath(const QString &path)
    {
        static const int MAXPATHLENGTH = 16;

        // Try to just get the filename from the path, and if it's longer than the max length return the whole thing
        QFileInfo fileInfo(path);
        if(!fileInfo.fileName().isEmpty()) {
            if(fileInfo.fileName().length() >= MAXPATHLENGTH) {
                return fileInfo.fileName();
            }
        }

        // If the filename is smaller than the max length, return a truncated path
        if(path.length() > MAXPATHLENGTH) {
            return QString("%1%2").arg(QChar(0x2026)).arg(path.right(MAXPATHLENGTH - 1));
        }

        return path;
    }
} //END anonymous namespace

namespace Plugins {
namespace OpenSpeedShop {

/*! \class Plugins::OpenSpeedShop::DataModel
    \brief A DataModel is used as the interface between a data view and the set
           of data it is meant to display to the user.

           This class takes care of the generation of the underlying data from
           an XML source, and the interaction with data views.
 */

/*! \fn DataModel::DataModel()
    \brief Constructs a DataModel with the given parent.
 */
DataModel::DataModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_Header = NULL;
    m_Uid = QUuid::createUuid();
}

/*! \fn DataModel::DataModel()
    \brief Constructs a DataModel with the given parent.
 */
DataModel::DataModel(QString xml, QObject *parent) :
    QAbstractItemModel(parent)
{
    m_Header = NULL;
    m_Uid = QUuid::createUuid();
    loadData(xml);
}

/*! \fn DataModel::DataModel()
    \brief Constructs a DataModel with the given parent.
 */
DataModel::DataModel(QDomDocument document, QObject *parent) :
  QAbstractItemModel(parent)
{
    m_Header = NULL;
    m_Uid = QUuid::createUuid();
    buildModel(document);
}

/*! \fn DataModel::~DataModel()
    \brief Destroys the DataModel.
 */
DataModel::~DataModel()
{
    if(m_Header) {
        delete(m_Header);
    }

    qDeleteAll(m_Rows);
}

QUuid DataModel::uid() const
{
    return m_Uid;
}

/*! \fn DataModel::loadData()
    \brief Builds a data model out of DataItems from the provided XML string.
    \param xml A QString containing the XML document object model that describes this DataModel.
 */
void DataModel::loadData(QString xml)
{
    QDomDocument document;
    document.setContent(xml);
    buildModel(document);
}

QString DataModel::dumpModel()
{
    QString dumpOut;
    foreach(Cell *row, m_Rows) {
        dumpOut.append(row->dump());
    }
    return dumpOut;
}

/*! \fn DataModel::buildModel()
    \brief Builds a data model out of DataItems from the provided XML document.
    \param document an XML document containing the description for this DataModel.
 */
void DataModel::buildModel(QDomDocument document)
{
    QDomElement responseElement = document.firstChildElement("Response");
    if(responseElement.isNull()) throw tr("'Response' element doesn't exist, as expected.");

    QDomElement ossElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(ossElement.isNull()) throw tr("'OpenSpeedShopCLI' element doesn't exist, as expected.");

    QDomElement commandObjectElement = ossElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) {

        // Deal with other responses, like errors
        QDomElement exceptionElement = ossElement.firstChildElement("Exception");
        if(!exceptionElement.isNull()) {
            throw tr("Exception returned from server: '%1'").arg(exceptionElement.text());
        }

        throw tr("'CommandObject' element doesn't exist, as expected. No error was given from the server.");
    }

    qDeleteAll(m_Rows);
    m_Rows.clear();

    int columnCount = 0;
    QDomElement rowElement = commandObjectElement.firstChildElement();
    while(!rowElement.isNull()) {

        QString rowType = rowElement.tagName();

        if(!rowType.compare("Headers", Qt::CaseInsensitive)) {

            Cell *cell = processCell(rowElement, NULL);
            if(cell) {
                columnCount = cell->columns.count();
                m_Header = cell;
            }

        } else if(!rowType.compare("Columns", Qt::CaseInsensitive)) {

            Cell *cell = processCell(rowElement, NULL);

            if(cell && cell->columns.count() == columnCount) {    // Ensure that we're not getting erroneous data from the CLI
                m_Rows.append(cell);

            } else {
                /* If you're seeing this warning, you have likely made a change to the CLI output that the GUI doesn't understand.
                   We are expecting that all Columns objects will have the same number of subobjects as the Headers object. */

                QString rowElementString;
                QTextStream tempStream(&rowElementString, QIODevice::WriteOnly);
                rowElement.save(tempStream, 2);
                qWarning() << tr("Unexpected data row size from CLI: \"%1\"").arg(rowElementString.replace('\n', ' ').remove('\r'));

            }

        } else {
            // Let's warn on other stuff, so the CLI guys know something is amiss when they test their changes in the GUI

            /* If you're getting this warning, you have made a change to the CLI output that the GUI doesn't understand.
               You can either remove your changes, or use exactly one Headers object along with multiple Columns objects to get your data into the GUI.
               If you need special exceptions, add them here; as is the case for the Enders object, which we're ignoring completely. */

            if(rowType.compare("Enders", Qt::CaseInsensitive)) {    // We're just plain ignoring Enders objects
                QString rowElementString;
                QTextStream tempStream(&rowElementString, QIODevice::WriteOnly);
                rowElement.save(tempStream, 2);
                qWarning() << tr("Unexpected element in CommandObject: \"%1\"").arg(rowElementString.replace('\n', ' ').remove('\r'));
            }

        }

        rowElement = rowElement.nextSiblingElement();
    }

}

DataModel::Cell *DataModel::processCell(QDomElement cellElement, Cell *parent)
{
    Cell *cell = new Cell(parent);
    cell->data["type"] = QVariant(cellElement.tagName());

    QDomNamedNodeMap attributes = cellElement.attributes();

#if QT_VERSION >= 0x050000
    int i = 0;
#else
    uint i = 0;
#endif

    for(i=0; i < attributes.length(); i++) {
        QDomAttr cellAttr = attributes.item(i).toAttr();
        if(!cellAttr.isNull()) {
            QString attrName = cellAttr.name();
            QString attrValue = cellAttr.value();

            // Do we know what type of value this is?
            QString valueType;
            if(!attrName.compare("value", Qt::CaseInsensitive)) {
                valueType = cell->data["type"].toString();
            }

            bool okay = false;
            QVariant value;
            if(valueType.isEmpty()) {
                // We don't know what type it is test for non-string values, and use those if we can
                if(!okay) value = attrValue.toULongLong(&okay);
                if(!okay) value = attrValue.toLongLong(&okay);
                if(!okay) value = attrValue.toDouble(&okay);

            } else if(!valueType.compare("Duration", Qt::CaseInsensitive) ||
                      !valueType.compare("Float", Qt::CaseInsensitive)) {
                value = attrValue.toDouble(&okay);

            } else if(!valueType.compare("Int", Qt::CaseInsensitive)) {
                value = attrValue.toLongLong(&okay);

            } else if(!valueType.compare("Interval", Qt::CaseInsensitive) ||
                      !valueType.compare("Uint", Qt::CaseInsensitive)) {
                value = attrValue.toULongLong(&okay);
            }

            // If it made it past all that and we still don't have a value type, just use the string
            if(!okay) value = attrValue;

            cell->data[attrName] = value;
        }
    }

    QDomElement childElement = cellElement.firstChildElement();
    while(!childElement.isNull()) {
        Cell *childCell = processCell(childElement, cell);
        if(childCell) cell->children.append(childCell);
        childElement = childElement.nextSiblingElement();
    }

    if(!parent) {
        cell->columns = cell->children;
        cell->children.clear();
    }

    return cell;
}

/*! \fn DataModel::saveData()
    \brief Generates an XML document object model from this DataModel's data set.
    \returns An XML document in flat text for easy file saving.
 */
QString DataModel::saveData() const
{
    //! \todo Complete the saveData function.
    return QString();
}


/* -=] QAbstractItemModel interface [=- */
/*! \fn DataModel::index()
    \brief Returns the index of the item in the model specified by the given
           row, column and parent index.

    \reimp Reimplemented from QAbstractItemModel.
    \sa createIndex()
 */
QModelIndex DataModel::index(int row, int column, const QModelIndex &parent) const
{
    /*! \note When reimplementing this function in a subclass, call
              createIndex() to generate model indexes that other components
              can use to refer to items in your model. */

    // Double check that we actually contain this index
    if(!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if(!parent.isValid()) {
        Cell *rowCell = m_Rows.at(row);
        Cell *cell = rowCell->columns.at(column);
        return createIndex(row, column, cell);
    } else {
        Cell *parentCell = static_cast<Cell *>(parent.internalPointer());
        Cell *cell = parentCell->children.at(row);
        return createIndex(row, column, cell);
    }

    // Return an empty index if there's no data for this request
    return QModelIndex();
}

/*! \fn DataModel::parent()
    \brief Returns the parent of the model item with the given index.

           If the item has no parent, an invalid QModelIndex is returned.

           A common convention used in models that expose tree data structures
           is that only items in the first column have children. For that
           case, when reimplementing this function in a subclass the column of
           the returned QModelIndex would be 0.

    \reimp Reimplemented from QAbstractItemModel.
    \sa createIndex()
 */
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

    Cell *childItem = static_cast<Cell *>(child.internalPointer());

    Cell *parentItem = childItem->parent;
    if(parentItem != NULL) {
        QString parentType = parentItem->data["type"].toString();

        if(!parentType.compare("Headers", Qt::CaseInsensitive) || !parentType.compare("Columns", Qt::CaseInsensitive)) {
            return QModelIndex();
        }

        return createIndex(parentItem->row(), parentItem->column(), parentItem);
    }

    return QModelIndex();
}

/*! \fn DataModel::rowCount()
    \brief Returns the number of rows under the given parent.

           When the parent is valid it means that rowCount is returning the
           number of children of parent.

    \reimp Reimplemented from QAbstractItemModel.
    \sa columnCount()
 */
int DataModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        return m_Rows.count();
    }

    Cell *parentItem = static_cast<Cell *>(parent.internalPointer());
    return parentItem->children.count();

    return 0;
}

/*! \fn DataModel::columnCount()
    \brief Returns the number of columns for the children of the given parent.

    \reimp Reimplemented from QAbstractItemModel.
    \sa rowCount()
 */
int DataModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        if(m_Header) {
            return m_Header->columns.count();
        }
        return 0;
    }

    Cell *parentItem = static_cast<Cell *>(parent.internalPointer());
    return parentItem->columns.count();

    return 0;
}

/*! \fn DataModel::data()
    \brief Returns the data stored under the given role for the item referred
           to by the index.

    \reimp Reimplemented from QAbstractItemModel.
    \sa Qt::ItemDataRole, setData(), and headerData()
 */
QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    Cell *dataItem = static_cast<Cell *>(index.internalPointer());

    if(role == Qt::DisplayRole) {
        return displayRole(dataItem);
    } else if(role == Qt::ToolTipRole) {
        return toolTipRole(dataItem);
    } else if(role == Qt::EditRole) {
        return editRole(dataItem);
    } else if(role == Qt::UserRole) {      //TODO: UID
        return QVariant();
    } else if(role == Qt::UserRole + 1) {  // Data type
        return dataItem->data["type"];
    } else if(role == Qt::UserRole + 2) {  // Additional data
        QString cellType = dataItem->data["type"].toString();
        if(!cellType.compare("CallStackEntry", Qt::CaseInsensitive)) {
            QStringList stack;
            foreach(Cell *child, dataItem->children) {
                stack.append(displayRole(child, false).toString());
            }
            return stack;
        }
    }

    return QVariant();
}

QVariant DataModel::displayRole(Cell *cell, bool truncate) const
{
    QString cellType = cell->data["type"].toString();

    if(!cellType.compare("CallStackEntry", Qt::CaseInsensitive)) {
        QString stack;
        int max = 5;
        int i = 0;
        while(i < max && i < cell->children.count()) {
            Cell *child = cell->children.at(i);
            stack.append(displayRole(child, truncate).toString());
            if(++i < max && i < cell->children.count()) {
                stack.append(QString(" %1 ").arg(QChar(0x00AB)));
            } else if(i >= max) {
                stack.append(QString(" %1 %2").arg(QChar(0x00AB)).arg(QChar(0x2026)));
            }
        }
        return stack;

    } else if(!cellType.compare("Function", Qt::CaseInsensitive)) {
        QString value = cell->data["value"].toString();
        if(cell->data.contains("path")) {
            QString path = cell->data["path"].toString();
            value.append(" (" + (truncate ? ::truncatePath(path) : path));
            if(cell->data.contains("line")) {
                value.append(":" + cell->data["line"].toString());
            }
            value.append(")");
        }
        return QVariant(value);

    } else if(!cellType.compare("Statement", Qt::CaseInsensitive)) {
        QString value;
        if(cell->data.contains("path")) {
            QString path = cell->data["path"].toString();
            value.append(truncate ? ::truncatePath(path) : path);
            if(cell->data.contains("line")) {
                value.append(":" + cell->data["line"].toString());
            }
        }
        return QVariant(value);
    }

    if(!cell->data.contains("value")) {
        return QVariant();
    }

    return cell->data["value"];
}

QVariant DataModel::toolTipRole(Cell *cell) const
{
    QString cellType = cell->data["type"].toString();
    if(!cellType.compare("CallStackEntry", Qt::CaseInsensitive)) {
        QString stack;
        int i=0;
        while(i < cell->children.count()) {
            Cell *child = cell->children.at(i);
            stack.append(displayRole(child, false).toString());
            if(++i < cell->children.count()) {
                stack.append(QChar::LineSeparator);
            }
        }
        return stack;
    }

    return QVariant();
}

QVariant DataModel::editRole(Cell *cell) const
{
    QString cellType = cell->data["type"].toString();
    if(!cellType.compare("CallStackEntry", Qt::CaseInsensitive)) {
        QString stack;
        int i = 0;
        while(i < cell->children.count()) {
            Cell *child = cell->children.at(i);
            stack.append(displayRole(child, false).toString());
            if(++i < cell->children.count()) {
                stack.append(" ");
            }
        }
        return stack;
    }

    return displayRole(cell, false);
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((orientation == Qt::Horizontal) && (m_Header) && (m_Header->columns.count() > section)) {
        Cell *header = m_Header->columns.at(section);
        if(role == Qt::DisplayRole) {
            return header->data["value"];
        } else if(role == Qt::ToolTipRole) {
            return header->data["columnType"];
        }
    }

//    if((orientation == Qt::Vertical) && (section < m_Rows.count())) {
//        Cell *row = m_Rows.at(section);
//        if(row && row->columns.count()) {
//            Cell *header = row->columns.last();
//            if(role == Qt::DisplayRole) {
//                return displayRole(header);
//            } else if(role == Qt::ToolTipRole) {
//                return toolTipRole(header);
//            }
//        }
//    }

    return QVariant();
}


} // namespace OpenSpeedShop
} // namespace Plugins
