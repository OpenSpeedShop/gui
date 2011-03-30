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

namespace Core {
namespace ActionManager {

/*!
   \class ActionManager
   \brief The ActionManager class is part of the core framework libraries, and
          manages the menus and toolbars in the MainWindow.

          singleton class
 */

ActionManager *m_Instance;

/*!
   \fn ActionManager::instance()
   \brief Access to the singleton instance of this class
   \returns A pointer to the singleton instance of this class
 */
ActionManager *ActionManager::instance()
{
    return m_Instance ? m_Instance : m_Instance = new ActionManager();
}

/*!
   \fn ActionManager::ActionManager()
   \brief Constructor
   \internal
 */
ActionManager::ActionManager() : QObject(0)
{
    m_Initialized = false;
}

/*!
   \fn ActionManager::~ActionManager()
   \brief Destructor
   \internal
 */
ActionManager::~ActionManager()
{
    if(m_Instance)
        m_Instance = NULL;
}

bool ActionManager::initialize()
{
    return m_Initialized = true;
}

bool ActionManager::initialized()
{
    return m_Initialized;
}

void ActionManager::shutdown()
{

}

void ActionManager::refreshMenuItems()
{
    QMenuBar *menuBar = MainWindow::MainWindow::instance()->menuBar();

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

void ActionManager::registerMenuItem(MenuItem *menuItem)
{
    m_MenuItems.append(menuItem);
    emit menuItemAdded(menuItem);
    refreshMenuItems();
}


}}
