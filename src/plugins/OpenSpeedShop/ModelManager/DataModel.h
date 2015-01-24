/*!
   \file DataModel.h
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2013 Argo Navis Technologies, LLC

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

#ifndef PLUGINS_OPENSPEEDSHOP_DATAMODEL_H
#define PLUGINS_OPENSPEEDSHOP_DATAMODEL_H

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QUuid>

#include "ModelManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class MODELMANAGER_EXPORT DataModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    explicit DataModel(QObject *parent = 0);
    explicit DataModel(QString xml, QObject *parent = 0);
    explicit DataModel(QDomDocument document, QObject *parent = 0);

    ~DataModel();

    QUuid uid() const;

    QString dumpModel();

    void loadData(QString xml);
    void buildModel(QDomDocument document);
    QString saveData() const;

    /* QAbstractItemModel interface */
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;


signals:

public slots:

protected:
    struct Cell {
        Cell(Cell *parent) { this->parent = parent; }
        ~Cell() { qDeleteAll(columns); qDeleteAll(children); }

        int row() {
            if(parent && parent->children.contains(this)) {
                return parent->children.indexOf(this);
            }
            return 0;
        }

        int column() {
            if(parent && parent->columns.contains(this)) {
                return parent->columns.indexOf(this);
            }
            return 0;
        }

        QString dump(int depth = 0) {
            QString dumpOut;

            QString tabs;
            for(int i=0; i<depth; i++) {
                tabs.append("->");
            }

            dumpOut.append(QString("%1 ---------------------------\n").arg(tabs));
            dumpOut.append(QString("%1Keys:\n").arg(tabs));
            foreach(QString key, data.keys()) {
                dumpOut.append(QString("%1  '%2' = '%3'\n").arg(tabs).arg(key).arg(data[key].toString()));
            }

            if(columns.count()) {
                dumpOut.append(QString("%1Columns:\n").arg(tabs));
                foreach(Cell *column, columns) {
                    dumpOut.append(column->dump(depth+1));
                }
            }

            if(children.count()) {
                dumpOut.append(QString("%1Children:\n").arg(tabs));
                foreach(Cell *child, children) {
                    dumpOut.append(child->dump(depth+1));
                }
            }

            return dumpOut;
        }

        Cell *parent;
        QMap<QString, QVariant> data;
        QList<Cell *> columns;
        QList<Cell *> children;
    };

    Cell *processCell(QDomElement cellElement, Cell *parent);
    Cell *m_Header;
    QList<Cell *> m_Rows;

    QVariant displayRole(Cell *cell, bool truncate = true) const;
    QVariant toolTipRole(Cell *cell) const;
    QVariant editRole(Cell *cell) const;

    QUuid m_Uid;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // PLUGINS_OPENSPEEDSHOP_DATAMODEL_H
