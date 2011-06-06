/*!
   \file ActionManager.cpp
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

#include "ActionManager.h"
#include "MainWindow/MainWindow.h"

#ifdef ACTIONMANAGER_DEBUG
#  include <QDebug>
#endif


namespace Core {
namespace ActionManager {

/*!
   \class ActionManager
   \brief The ActionManager class is part of the core framework libraries, and
          manages the menus and toolbars in the MainWindow.

          singleton class
 */

/*!
   \fn ActionManager::instance()
   \brief Access to the singleton instance of this class
   \returns A reference to the singleton instance of this class
 */
ActionManager &ActionManager::instance()
{
    static ActionManager m_Instance;
    return m_Instance;
}

/*!
   \fn ActionManager::ActionManager()
   \brief Constructor
 */
ActionManager::ActionManager() : QObject(0)
{
    m_Initialized = false;
}

/*!
   \fn ActionManager::~ActionManager()
   \brief Destructor
 */
ActionManager::~ActionManager()
{
}

/*!
   \fn ActionManager::initialize()
   \brief Initializes this class after it has been constructed.
   This design pattern allows the class to perform any operations after a class that this object is dependent upon has been
   constructed.
   \returns true if successful.
 */
bool ActionManager::initialize()
{
    return m_Initialized = true;
}

/*!
   \fn ActionManager::initialized()
   \returns A boolean value indicating whether this instance has been initialized or not.
 */
bool ActionManager::initialized()
{
    return m_Initialized;
}

/*!
   \fn ActionManager::shutdown()
   \brief Notifies the instance that it should perform any clean-up operations before destruction.
   This method is called manually, before the application is closed.  It will occur before destruction of the instance.
 */
void ActionManager::shutdown()
{
}

/*!
   \fn ActionManager::refreshMenuItems()
   \brief This has not been implemented fully.
 */
void ActionManager::refreshMenuItems()
{
    QMenuBar *menuBar = MainWindow::MainWindow::instance().menuBar();

    QList<MenuItem *> menuItems(m_MenuItems);
    QList<MenuItem *> base;

    // Merge all of the items in the list
    while(!menuItems.isEmpty()) {
        MenuItem *menuItem = menuItems.takeFirst();
        bool newMenuItem = false;
        foreach(MenuItem *merge, menuItems) {
            if(menuItem->action()->text() == merge->action()->text()) {
                if(newMenuItem) delete menuItem;
                newMenuItem = true;
                menuItem = menuItem->merge(merge);
                menuItems.removeOne(merge);
            }
        }
        base.append(menuItem);
    }

    // Remove and delete all actions
    while(!menuBar->actions().isEmpty()) {
        QAction *action = menuBar->actions().first();
        menuBar->removeAction(action);

        // Do I need to actually do this? or does the QAction handle the QMenu?
        if(action->menu()) {
            QMenu *menu = action->menu();
            action->setMenu(NULL);
            delete menu;
        }

        delete action;
    }

    // Add them by priority into the MainWindow's MenuBar
    qSort(base.begin(), base.end(), MenuItem::ascending);
    MenuItem *menuItem = NULL;
    while(!base.isEmpty()) {
        menuItem = base.takeFirst();
        menuBar->addAction(menuItem->generate());
    }
}

/*!
   \fn ActionManager::refreshMenuItems()
   \brief This has not been implemented fully.
 */
void ActionManager::registerMenuItem(MenuItem *menuItem)
{
    m_MenuItems.append(menuItem);
    emit menuItemAdded(menuItem);
    refreshMenuItems();
}


}}
