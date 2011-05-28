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

#include "Delegate.h"

#include <QApplication>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

namespace Plugins {
namespace OpenSpeedShop {

/*! \class Plugins::OpenSpeedShop::Delegate
    \brief A basic delegate to assist the tree and table views in the set of standard views available for the main
           OpenSpeeShop plugin.
    \sa QStyledItemDelegate QAbstractItemDelegate
 */

/*! \fn Delegate::Delegate()
    \brief Constructor.
 */
Delegate::Delegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

/*! \fn Delegate::paint()
    \brief Handles special case items (like CallStackEntry and percentage values).
    \reimp QStyledItemDelegate::paint()
    \sa sizeHint() selected() deselected()
 */
void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString indexType = index.data(Qt::UserRole+1).toString();
    QString headerTitle = index.model()->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
    if(indexType.compare("Float", Qt::CaseInsensitive) == 0 && headerTitle.contains("%")) {
        double progress = index.data().toDouble();

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = (int)progress;
        progressBarOption.text = QString::number(progress, 'f', 3) + "%";
        progressBarOption.textVisible = true;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
        return;


    // If it's a CallStackEntry and it's a selected row, show the full stack trace
    } else if(indexType.compare("CallStackEntry") == 0 && m_SelectedRows.contains(index)) {
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

    QStyledItemDelegate::paint(painter, option, index);
}

/*! \fn Delegate::sizeHint()
    \brief Handles special case items (like CallStackEntry).
    \reimp QStyledItemDelegate::sizeHint()
    \sa print() selected() deselected()
 */
QSize Delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    /* QTBUG: Within this function, options lacks the QStyle::State_Selected flag in all possible call paths. This is
       apparently a known bug and nobody seems to give a crap about it.  The work-around was to create a notifying TreeView
       that lets the delegate keep track of the selected objects in the model.

       http://www.qtcentre.org/threads/25045-Different-size-hint-for-selected-cells-in-QListItemView
     */

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
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
            widget = v3->widget;

        QStyle *style = widget ? widget->style() : QApplication::style();
        return style->sizeFromContents(QStyle::CT_ItemViewItem, &opt, QSize(), widget);
    }

    return QStyledItemDelegate::sizeHint(option, index);
}

/*! \fn Delegate::selected()
    \brief Keeps track of selected items, and notifies the view of any size changes.
    This is part of a work around involving the known bug detailed in Delegate::sizeHint().
    \sa deselected() sizeHint() print() TreeView::selectionChanged()
 */
void Delegate::selected(const QModelIndex &index)
{
    if(!m_SelectedRows.contains(index)) {
        m_SelectedRows.insert(index);
        emit sizeHintChanged(index);
    }
}


/*! \fn Delegate::deselected()
    \brief Keeps track of selected items, and notifies the view of any size changes.
    This is part of a work around involving the known bug detailed in Delegate::sizeHint().
    \sa selected() sizeHint() print() TreeView::selectionChanged()
 */
void Delegate::deselected(const QModelIndex &index)
{
    if(m_SelectedRows.contains(index)) {
        m_SelectedRows.remove(index);
        emit sizeHintChanged(index);
    }
}

/*! \fn Delegate::helpEvent()
    \brief Handles special help items (like tooltips) for special items like CallStackEntries
    \reimp QStyledItemDelegate::helpEvent()
    \sa QAbstractItemDelegate::helpEvent() QStyledItemDelegate::helpEvent()
 */
bool Delegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Ignore tooltip requests for selected CallStackEntries; we're already showing the full trace
    QString indexType = index.data(Qt::UserRole+1).toString();
    if(event->type() == QEvent::ToolTip &&
            indexType.compare("CallStackEntry") == 0 &&
            m_SelectedRows.contains(index))
    {
        return false;
    }

    return QStyledItemDelegate::helpEvent(event, view, option, index);
}



} // namespace OpenSpeedShop
} // namespace Plugins
