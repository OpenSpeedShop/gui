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

#include "MenuItem.h"

namespace Core {
namespace ActionManager {

MenuItem::MenuItem(QWidget *parent) :
    QMenu(parent)
{
    setPriority(128);
}

int MenuItem::priority()
{
    return menuActionItem()->priority();
}

void MenuItem::setPriority(int priority)
{
    menuActionItem()->setPriority(priority);
}

MenuItem *MenuItem::merge(MenuItem* menu)
{
    return merge(this, menu);
}

MenuItem *MenuItem::merge(MenuItem* left, MenuItem *right)
{
    MenuItem *retval = new MenuItem();

    // Merge this object
    retval->setTitle( left->title().isEmpty() ? right->title() : left->title() );
    retval->setToolTip( left->toolTip().isEmpty() ? right->toolTip() : left->toolTip() );
    retval->setWhatsThis( left->whatsThis().isEmpty() ? right->whatsThis() : left->whatsThis() );
    retval->setIcon( left->icon().isNull() ? right->icon() : left->icon() );
    retval->setPriority( left->priority() == 128 ? right->priority() : left->priority() );

    // Now, merge the children
    QList<ActionItem *> leftActionItems = left->actionItems();
    QList<ActionItem *> rightActionItems = right->actionItems();

    foreach(ActionItem *leftAction, leftActionItems) {
        foreach(ActionItem *rightAction, rightActionItems) {
            if( (leftAction->text() == rightAction->text()) || (leftAction->objectName() == rightAction->objectName()) ) {
                if(leftAction->menuItem()) {
                    MenuItem *menuItem = leftAction->menuItem()->merge(rightAction->menuItem());
                    menuItem->setParent(retval);    // We only parent cloned actions for disposal
                    retval->addMenuItem(menuItem);
                } else {
                    ActionItem *actionItem = leftAction->merge(rightAction);
                    actionItem->setParent(retval);    // We only parent cloned actions for disposal
                    retval->addAction(actionItem);
                }

                leftActionItems.removeOne(leftAction);
                rightActionItems.removeOne(rightAction);

                break;
            }
        }
    }

    // Add any children that didn't get merged.
    // Remember, not to adopt these children, we don't own them!
    foreach(ActionItem *leftAction, leftActionItems)
        retval->addAction(leftAction);
    foreach(ActionItem *rightAction, rightActionItems)
        retval->addAction(rightAction);

    return retval;
}

QList<ActionItem *> MenuItem::actionItems()
{
    QList<ActionItem *> actionItems;

    foreach(QAction *action, actions())
        actionItems.append((ActionItem *)action);

    return actionItems;
}

ActionItem *MenuItem::menuActionItem()
{
    return (ActionItem *)menuAction();
}

void MenuItem::addActionItem(ActionItem *actionItem)
{
    foreach(ActionItem *item, actionItems()) {
        if(item->priority() > actionItem->priority()) {
            insertAction(item, actionItem);
            return;
        }
    }

    addAction(actionItem);
}

void MenuItem::addMenuItem(MenuItem *menuItem)
{
    foreach(ActionItem *item, actionItems()) {
        if(item->priority() > menuItem->priority()) {
            insertMenu(item, menuItem);
            return;
        }
    }
    addMenu(menuItem);
}

} // namespace ActionManager
} // namespace Core
