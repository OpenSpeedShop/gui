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

#ifndef MENUITEM_H
#define MENUITEM_H

#include <QObject>
#include <QMenu>
#include <QAction>

namespace Core {
namespace ActionManager {

enum MenuItemTypes {
    MenuItemType_SubMenu,
    MenuItemType_Group,
    MenuItemType_Separator,
    MenuItemType_Action
};

class MenuItem : public QObject
{
    Q_OBJECT
public:
    explicit MenuItem(QObject *parent = 0);
    MenuItem(MenuItem *menuItem, QObject *parent = 0);

    int priority();
    void setPriority(int priority);

    QAction *action();

    MenuItemTypes menuItemType();

    QList<MenuItem *> menuItems();
    void addMenuItem(MenuItem *menuItem);

    MenuItem *merge(MenuItem* menuItem);
    static MenuItem *merge(MenuItem* left, MenuItem *right);

    QAction *generate();

    static bool ascending(MenuItem *left, MenuItem *right) { return left->priority() < right->priority(); }

signals:

public slots:

protected:
    QList<MenuItem *> m_MenuItems;
    QAction *m_Action;
    int m_Priority;
};

} // namespace ActionManager
} // namespace Core

#endif // MENUITEM_H
