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
}

/*!
   \fn ActionManager::~ActionManager()
   \brief Destructor
   \internal
 */
ActionManager::~ActionManager()
{
}

void ActionManager::registerMenuItem(QString menuName, QAction *action, int priority)
{
    Q_UNUSED(priority);

    /* Replace any ampersands that might be in the name, so we can have a
       "clean" name for object searching */
    QString objectName(menuName.replace('&', "").toLower());

    QMenu *menu = NULL;
    foreach (QMenu *m, m_Menus) {
        if (m->objectName() == objectName) {
            menu = m;
        }
    }

    if (!menu) {
        menu = new QMenu(menuName);
        menu->setObjectName(objectName);

        m_Menus.append(menu);
        emit menuAdded(menu);
    }

    menu->addAction(action);
}





}}
