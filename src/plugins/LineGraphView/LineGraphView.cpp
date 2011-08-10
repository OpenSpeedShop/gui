/*!
   \file LineGraphView.cpp
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

#include "LineGraphView.h"

namespace Plugins {
namespace LineGraphView {

/*! \class Plugins::LineGraphView::LineGraphView
    \brief One in the set of standard views available for the main OpenSpeeShop plugin.
    \sa QLineGraphView QAbstractItemView
 */


/*! \fn LineGraphView::LineGraphView()
    \brief Constructor.
 */
LineGraphView::LineGraphView(QWidget *parent) :
    QLineGraphView(parent)
{
    m_ProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_ProxyModel.setFilterRole(Qt::EditRole);
    m_ProxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    m_ProxyModel.setSortRole(Qt::EditRole);
    QLineGraphView::setModel(&m_ProxyModel);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setSortingEnabled(true);

    QAbstractItemDelegate *oldDelegate = itemDelegate();
    setItemDelegate(&m_ItemDelegate);
    oldDelegate->deleteLater();
}

LineGraphView::~LineGraphView()
{
}


/*! \fn LineGraphView::model()
    \brief Reimplemented in order to wrap the model in a proxy for easier filtering and sorting.
    \reimp QLineGraphView::model()
    \sa QLineGraphView::model LineGraphView::setModel
 */
QAbstractItemModel *LineGraphView::model() const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QLineGraphView::model());
    return proxyModel->sourceModel();
}

/*! \fn LineGraphView::setModel()
    \brief Reimplemented in order to wrap the model in a proxy for easier filtering and sorting.
    \reimp QLineGraphView::setModel()
    \sa QLineGraphView::setModel LineGraphView::model
 */
void LineGraphView::setModel(QAbstractItemModel *model)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QLineGraphView::model());
    proxyModel->setSourceModel(model);

    for(int i = 0; i < proxyModel->columnCount(); i++) {
        resizeColumnToContents(i);
    }
}

/*! \fn LineGraphView::selectionChanged()
    \brief Reimplemented in order to notify the Delegate of any selection changes.
    \reimp QLineGraphView::selectionChanged()
    \sa QLineGraphView::selectionChanged Delegate::selected() Delegate::deselected()
 */
void LineGraphView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
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

    QLineGraphView::selectionChanged(selected, deselected);
}

QString LineGraphView::viewFilter() const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QLineGraphView::model());
    return proxyModel->filterRegExp().pattern();
}

void LineGraphView::setViewFilter(const QString &regex)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QLineGraphView::model());
    selectionModel()->clear();
    proxyModel->setFilterRegExp(regex);
}

int LineGraphView::viewFilterColumn() const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QLineGraphView::model());
    return proxyModel->filterKeyColumn();
}

void LineGraphView::setViewFilterColumn(int column)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QLineGraphView::model());
    selectionModel()->clear();
    proxyModel->setFilterKeyColumn(column);
}

} // namespace LineGraphView
} // namespace Plugins
