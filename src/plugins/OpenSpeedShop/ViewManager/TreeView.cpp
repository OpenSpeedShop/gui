/*!
   \file TreeView.cpp
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

#include "TreeView.h"
#include "Delegate.h"

#include <QSortFilterProxyModel>

namespace Plugins {
namespace OpenSpeedShop {

/*! \class Plugins::OpenSpeedShop::TreeView
    \brief One in the set of standard views available for the main OpenSpeeShop plugin.
    \sa QTreeView QAbstractItemView
 */


/*! \fn TreeView::TreeView()
    \brief Constructor.
 */
TreeView::TreeView(QWidget *parent) :
    QTreeView(parent)
{
    QAbstractItemDelegate *oldDelegate = itemDelegate();
    setItemDelegate(new Delegate(this));
    oldDelegate->deleteLater();
}

/*! \fn TreeView::model()
    \brief Reimplemented in order to wrap the model in a proxy for easier filtering and sorting.
    \reimp QTreeView::model()
    \sa QTreeView::model TreeView::setModel
 */
QAbstractItemModel *TreeView::model() const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());
    if(proxyModel) {
        return proxyModel->sourceModel();
    }

    return NULL;
}

/*! \fn TreeView::setModel()
    \brief Reimplemented in order to wrap the model in a proxy for easier filtering and sorting.
    \reimp QTreeView::setModel()
    \sa QTreeView::setModel TreeView::model
 */
void TreeView::setModel(QAbstractItemModel *model)
{
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    QTreeView::setModel(proxyModel);
    setSortingEnabled(true);
}

/*! \fn TreeView::selectionChanged()
    \brief Reimplemented in order to notify the Delegate of any selection changes.
    \reimp QTreeView::selectionChanged()
    \sa QTreeView::selectionChanged Delegate::selected() Delegate::deselected()
 */
void TreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Delegate *delegate = qobject_cast<Delegate *>(itemDelegate());
    if(delegate) {
        foreach(QModelIndex index, selected.indexes()) {
            delegate->selected(index);
        }

        foreach(QModelIndex index, deselected.indexes()) {
            delegate->deselected(index);
        }
    }

    QTreeView::selectionChanged(selected, deselected);
}

} // namespace OpenSpeedShop
} // namespace Plugins
