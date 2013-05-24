/*!
   \file
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

#ifndef TREEVIEWM_H
#define TREEVIEWM_H

#include <QTreeView>
#include <QSortFilterProxyModel>

#include <ViewManager/IViewFilterable.h>

#include "Delegate.h"

namespace Plugins {
namespace TreeView {

class TreeView : public QTreeView, public Core::ViewManager::IViewFilterable
{
    Q_OBJECT
    Q_INTERFACES(Core::ViewManager::IViewFilterable)

public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QString viewFilter() const;
    void setViewFilter(const QString &regex);
    int viewFilterColumn() const;
    void setViewFilterColumn(int column = 0);

protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected:
    Delegate m_ItemDelegate;
    QSortFilterProxyModel m_ProxyModel;

};

} // namespace TreeView
} // namespace Plugins

#endif // TREEVIEWM_H
