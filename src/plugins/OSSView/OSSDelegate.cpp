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

#include "OSSDelegate.h"

#include <QApplication>
#include <QHelpEvent>

namespace Plugins {
namespace OSSView {

/*! \class Plugins::OSSView::OSSDelegate
    \brief A basic delegate to assist the tree and table views in the set of standard views available for the main
           OpenSpeeShop plugin.
    \sa QStyledItemDelegate QAbstractItemDelegate
 */

/*! \fn OSSDelegate::OSSDelegate()
    \brief Constructor.
 */
OSSDelegate::OSSDelegate(QObject *parent) :
    Plugins::TableView::Delegate(parent)
{
}

/*! \fn OSSDelegate::paint()
    \brief Handles special case items (like CallStackEntry and percentage values).
    \reimp QStyledItemDelegate::paint()
    \sa sizeHint() selected() deselected()
 */
void OSSDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString indexType = index.data(Qt::UserRole+1).toString();

    // If it's a CallStackEntry and it's a selected row, show the full stack trace
    if(indexType.compare("CallStackEntry") == 0 && m_SelectedRows.contains(index)) {
        QStyleOptionViewItemV4 opt = option;
        initStyleOption(&opt, index);

        QStringList stringList = index.data(Qt::UserRole+2).toStringList();
        opt.text.clear();
        for(int i=0; i < stringList.count(); i++) {
            opt.text.append(stringList.at(i));
            if(i < stringList.length()-1) {
                opt.text.append(QChar::LineSeparator);
            }
        }

        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
        return;
    }

    Plugins::TableView::Delegate::paint(painter, option, index);
}

/*! \fn OSSDelegate::sizeHint()
    \brief Handles special case items (like CallStackEntry).
    \reimp QStyledItemDelegate::sizeHint()
    \sa print() selected() deselected()
 */
QSize OSSDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // If it's a CallStackEntry and it's a selected row, show the full stack trace
    QString indexType = index.data(Qt::UserRole+1).toString();
    if(indexType.compare("CallStackEntry") == 0 && m_SelectedRows.contains(index)) {
        QStyleOptionViewItemV4 opt = option;
        initStyleOption(&opt, index);

        QStringList stringList = index.data(Qt::UserRole+2).toStringList();
        opt.text.clear();
        for(int i=0; i < stringList.count(); i++) {
            opt.text.append(stringList.at(i));
            if(i < stringList.length()-1) {
                opt.text.append(QChar::LineSeparator);
            }
        }

        const QWidget *widget = NULL;
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option)) {
            widget = v3->widget;
        }

        QStyle *style = widget ? widget->style() : QApplication::style();
        return style->sizeFromContents(QStyle::CT_ItemViewItem, &opt, QSize(), widget);
    }

    return Plugins::TableView::Delegate::sizeHint(option, index);
}


/*! \fn OSSDelegate::helpEvent()
    \brief Handles special help items (like tooltips) for special items like CallStackEntries
    \reimp QStyledItemDelegate::helpEvent()
    \sa QAbstractItemDelegate::helpEvent() QStyledItemDelegate::helpEvent()
 */
bool OSSDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Ignore tooltip requests for selected CallStackEntries; we're already showing the full trace
    QString indexType = index.data(Qt::UserRole+1).toString();
    if(event->type() == QEvent::ToolTip &&
            indexType.compare("CallStackEntry") == 0 &&
            m_SelectedRows.contains(index))
    {
        return false;
    }

    return Plugins::TableView::Delegate::helpEvent(event, view, option, index);
}



} // namespace OSSView
} // namespace Plugins
