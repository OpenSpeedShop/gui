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

#include "ActionItem.h"
#include "MenuItem.h"

namespace Core {
namespace ActionManager {

ActionItem::ActionItem(QObject *parent) :
    QAction(parent),
    m_Priority(128)
{
}

ActionItem::ActionItem(const QString &text, QObject *parent) :
    QAction(text, parent),
    m_Priority(128)
{
}

ActionItem::ActionItem(const QIcon &icon, const QString &text, QObject *parent) :
    QAction(icon, text, parent),
    m_Priority(128)
{
}

int ActionItem::priority()
{
    return m_Priority;
}

void ActionItem::setPriority(int priority)
{
    m_Priority = priority;
}

MenuItem *ActionItem::menuItem()
{
    return (MenuItem *)menu();
}

ActionItem *ActionItem::merge(ActionItem *other)
{
    return ActionItem::merge(this, other);
}

ActionItem *ActionItem::merge(ActionItem *left, ActionItem *right)
{
    ActionItem *retval = new ActionItem();

    retval->setText( left->text().isEmpty() ? right->text() : left->text() );
    retval->setObjectName( left->objectName().isEmpty() ? right->objectName() : left->objectName() );
    retval->setToolTip( left->toolTip().isEmpty() ? right->toolTip() : left->toolTip() );
    retval->setWhatsThis( left->whatsThis().isEmpty() ? right->whatsThis() : left->whatsThis() );
    retval->setIcon( left->icon().isNull() ? right->icon() : left->icon() );
    retval->setPriority( left->priority() == 128 ? right->priority() : left->priority() );
    retval->setCheckable(left->isCheckable() || right->isCheckable());
    retval->setSeparator(left->isSeparator() || right->isSeparator());

    connect(retval, SIGNAL(toggled(bool)), left, SLOT(toggle()));
    connect(retval, SIGNAL(toggled(bool)), right, SLOT(toggle()));
    connect(retval, SIGNAL(triggered()), left, SLOT(trigger()));
    connect(retval, SIGNAL(triggered()), right, SLOT(trigger()));
    connect(retval, SIGNAL(hovered()), left, SLOT(hover()));
    connect(retval, SIGNAL(hovered()), right, SLOT(hover()));

    return retval;
}


} // namespace ActionManager
} // namespace Core
