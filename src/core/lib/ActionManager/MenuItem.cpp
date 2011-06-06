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

/*!
   \class Core::ActionManager::MenuItem
   \brief This class wraps Qt4 menu and action items within the to more easily facilitate menu systems within the
          GUI framework.
   Not fully implemented.
 */


/*!
   \fn MenuItem::MenuItem()
   \brief Constructor.
   Not fully implemented.
 */
MenuItem::MenuItem(QObject *parent) :
    QObject(parent),
    m_Action(this)
{
    m_Priority = 128;
}

/*!
   \fn MenuItem::MenuItem()
   \brief Constructor
   Not fully implemented.
 */
MenuItem::MenuItem(MenuItem *menuItem, QObject *parent) :
    QObject(parent),
    m_Action(this)
{
    m_Priority = menuItem->priority();

    m_Action.setText(menuItem->action()->text());
    m_Action.setToolTip(menuItem->action()->toolTip());
    m_Action.setWhatsThis(menuItem->action()->whatsThis());
    m_Action.setIcon(menuItem->action()->icon());
    m_Action.setCheckable(menuItem->action()->isCheckable());
    m_Action.setSeparator(menuItem->action()->isSeparator());
}

/*!
   \fn MenuItem::~MenuItem()
   \brief Destructor.
   Not fully implemented.
 */
QAction *MenuItem::action()
{
    return &m_Action;
}

/*!
   \fn MenuItem::priority()
   \brief Access to the priority property.
   Not fully implemented.
   \returns An integer representing the priority of this object.
 */
int MenuItem::priority()
{
    return m_Priority;
}

/*!
   \fn MenuItem::setPriority()
   \brief Access to the priority property. Sets the priority property to the given integer value.
   Not fully implemented.
 */
void MenuItem::setPriority(int priority)
{
    m_Priority = priority;
}

/*!
   \fn MenuItem::type()
   \brief Access to the type property.
   Not fully implemented.
   \returns The type of object that this represents.
   \sa MenuItem::Types
 */
MenuItem::Types MenuItem::type()
{
    if(action()->actionGroup())
        return Type_Group;
    if(action()->menu())
        return Type_SubMenu;
    if(action()->isSeparator())
        return Type_Separator;
    return Type_Action;
}

/*!
   \fn MenuItem::menuItems()
   \brief Gets a list of child menu items
   Not fully implemented.
 */
QList<MenuItem *> MenuItem::menuItems()
{
    return m_MenuItems;
}

/*!
   \fn MenuItem::addMenuItem()
   \brief Adds a menu item to the child list
   Not fully implemented.
 */
void MenuItem::addMenuItem(MenuItem *menuItem)
{
    for(int i=0; i < m_MenuItems.count(); i++) {
        if(m_MenuItems[i]->priority() > menuItem->priority()) {
            m_MenuItems.insert(i, menuItem);
            return;
        }
    }
    m_MenuItems.append(menuItem);
}

/*!
   \fn MenuItem::merge()
   \brief Merges another menu with this object, and returns a new menu set.
   Not fully implemented.
 */
MenuItem *MenuItem::merge(MenuItem* menuItem)
{
    return merge(this, menuItem);
}

/*!
   \fn MenuItem::merge()
   \brief Merges two menu objects, and returns a new menu set.
   Not fully implemented.
 */
MenuItem *MenuItem::merge(MenuItem* left, MenuItem *right)
{
    MenuItem *retval = new MenuItem();

    // Merge the two MenuItems
    retval->action()->setText( left->action()->text().isEmpty() ? right->action()->text() : left->action()->text() );
    retval->action()->setToolTip( left->action()->toolTip().isEmpty() ? right->action()->toolTip() : left->action()->toolTip() );
    retval->action()->setWhatsThis( left->action()->whatsThis().isEmpty() ? right->action()->whatsThis() : left->action()->whatsThis() );
    retval->action()->setIcon( left->action()->icon().isNull() ? right->action()->icon() : left->action()->icon() );
    retval->action()->setCheckable(left->action()->isCheckable() || right->action()->isCheckable());
    retval->action()->setSeparator(left->action()->isSeparator() || right->action()->isSeparator());
    retval->setPriority( left->priority() == 128 ? right->priority() : left->priority() );

    //! \todo We need to figure out how to support groups!

    // Merge the submenu items
    if(left->type() == Type_SubMenu) {
        // Merge the children
        QList<MenuItem *> leftMenuItems = left->menuItems();
        QList<MenuItem *> rightMenuItems = right->menuItems();

        // Merge everything we can find matches for
        foreach(MenuItem *leftMenuItem, leftMenuItems) {
            foreach(MenuItem *rightMenuItem, rightMenuItems) {
                if(leftMenuItem->action()->text() == rightMenuItem->action()->text()) {
                    MenuItem *merged = merge(leftMenuItem, rightMenuItem);
                    merged->setParent(retval);
                    retval->addMenuItem(merged);

                    leftMenuItems.removeOne(leftMenuItem);
                    rightMenuItems.removeOne(rightMenuItem);

                    break;
        } } }

        // Add any children that didn't get merged
        foreach(MenuItem *leftMenuItem, leftMenuItems)
            retval->addMenuItem(new MenuItem(leftMenuItem, retval));
        foreach(MenuItem *rightMenuItem, rightMenuItems)
            retval->addMenuItem(new MenuItem(rightMenuItem, retval));
    }

    return retval;
}

/*!
   \fn MenuItem::generate()
   Not fully implemented.
 */
QAction *MenuItem::generate()
{
    QAction *retval = NULL;

    if(type() == Type_SubMenu) {
        QMenu *menu = new QMenu();
        retval = menu->menuAction();
    } else {
        retval = new QAction(NULL);
    }

    retval->setText(action()->text());
    retval->setToolTip(action()->toolTip());
    retval->setWhatsThis(action()->whatsThis());
    retval->setIcon(action()->icon());
    retval->setCheckable(action()->isCheckable());
    retval->setSeparator(action()->isSeparator());

    if(type() == Type_SubMenu) {
        foreach(MenuItem *menuItem, menuItems()) {
            QAction *action = menuItem->generate();
            action->setParent(retval);
            retval->menu()->addAction(action);
        }
    }

    return retval;
}



} // namespace ActionManager
} // namespace Core
