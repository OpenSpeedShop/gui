/*!
   \file MainWindow.cpp
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

#ifdef QT_DEBUG
  #include <QtDebug>
#endif

#include "MainWindow.h"

namespace Core {
namespace MainWindow {

MainWindow *m_Instance;

/*!
   \class Core::MainWindow
   \brief The MainWindow class is part of the core framework libraries. It is
          the actual viewport that the user sees. All plugins manipulate this
          view to expose data to the user.

          singleton class
 */

/*!
   \fn Core::MainWindow::instance()
   \brief Access to the singleton instance of this class
   \returns A pointer to the singleton instance of this class
 */
MainWindow *MainWindow::instance()
{
    return m_Instance ? m_Instance : m_Instance = new MainWindow();
}

/*!
   \fn Core::MainWindow::MainWindow()
   \brief Constructor
   \internal
 */
MainWindow::MainWindow() : QMainWindow(NULL)
{
    m_Initialized = false;

    // Easier notification (than the alternatives) that we're closing
    setAttribute( Qt::WA_DeleteOnClose );

    setWindowIcon(QIcon(":/MainWindow/app.png"));
}

/*!
   \fn Core::MainWindow::MainWindow()
   \brief Destructor
   \internal
 */
MainWindow::~MainWindow()
{
    if(m_Instance)
        m_Instance = NULL;
}

bool MainWindow::initialize()
{
    readSettings();
    initActions();

    return m_Initialized = true;
}

bool MainWindow::initialized()
{
    return m_Initialized;
}

void MainWindow::shutdown()
{
    writeSettings();
}

/*!
   \fn Core::MainWindow::readSettings()
   \brief Load settings from the SettingManager.
   \internal
 */
void MainWindow::readSettings()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("MainWindow");

    resize( settingManager->value("WindowSize", QSize(640, 400)).toSize() );
    move( settingManager->value("WindowPosition", QPoint(0, 0)).toPoint() );

    settingManager->endGroup();
}

/*!
   \fn Core::MainWindow::writeSettings()
   \brief Stores settings in the SettingManager for later retrieval.
   \internal
 */
void MainWindow::writeSettings()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("MainWindow");

    settingManager->setValue("WindowSize", size());
    settingManager->setValue("WindowPosition", pos());

    settingManager->endGroup();
}

void MainWindow::initActions()
{
    ActionManager::ActionManager *actionManager =
            ActionManager::ActionManager::instance();

    // Create base (default) menu layout with top level MenuItems and priority levels
    ActionManager::MenuItem *fileMenu = new ActionManager::MenuItem(this);
    fileMenu->setTitle(tr("File"));
    fileMenu->setPriority(16);
    actionManager->registerMenuItem(fileMenu);

    ActionManager::MenuItem *editMenu = new ActionManager::MenuItem(this);
    editMenu->setTitle(tr("Edit"));
    editMenu->setPriority(32);
    actionManager->registerMenuItem(editMenu);

    ActionManager::MenuItem *toolsMenu = new ActionManager::MenuItem(this);
    toolsMenu->setTitle(tr("Tools"));
    toolsMenu->setPriority(208);
    actionManager->registerMenuItem(toolsMenu);

    ActionManager::MenuItem *windowMenu = new ActionManager::MenuItem(this);
    windowMenu->setTitle(tr("Window"));
    windowMenu->setPriority(224);
    actionManager->registerMenuItem(windowMenu);

    ActionManager::MenuItem *helpMenu = new ActionManager::MenuItem(this);
    helpMenu->setTitle(tr("Help"));
    helpMenu->setPriority(240);
    actionManager->registerMenuItem(helpMenu);


    // Create and connect actions to local slots
    // Register actions in menus


    ActionManager::ActionItem *action;

    ActionManager::MenuItem *left =
            new ActionManager::MenuItem(this);
    left->setTitle("Edit");

    action = new ActionManager::ActionItem(left);
    action->setText("Undo/Redo");
    action->setSeparator(true);
    left->addActionItem(action);
    action = new ActionManager::ActionItem(left);
    action->setText("Undo");
    left->addActionItem(action);
    action = new ActionManager::ActionItem(left);
    action->setText("Redo");
    left->addActionItem(action);

    action = new ActionManager::ActionItem(left);
    action->setText("Edit Actions");
    action->setSeparator(true);
    left->addActionItem(action);
    action = new ActionManager::ActionItem(left);
    action->setText("Paste");
    left->addActionItem(action);
    action = new ActionManager::ActionItem(left);
    action->setText("Copy");
    left->addActionItem(action);
    action = new ActionManager::ActionItem(left);
    action->setText("Cut");
    left->addActionItem(action);

    action = new ActionManager::ActionItem(left);
    action->setText("Left Only!");
    left->addActionItem(action);

    actionManager->registerMenuItem(left);

    ActionManager::MenuItem *right =
            new ActionManager::MenuItem(this);
    right->setTitle("Edit");

    action = new ActionManager::ActionItem(right);
    action->setText("Undo/Redo");
    action->setSeparator(true);
    right->addActionItem(action);
    action = new ActionManager::ActionItem(right);
    action->setText("Undo");
    right->addActionItem(action);
    action = new ActionManager::ActionItem(right);
    action->setText("Redo");
    right->addActionItem(action);

    action = new ActionManager::ActionItem(right);
    action->setText("Edit Actions");
    action->setSeparator(true);
    right->addActionItem(action);
    action = new ActionManager::ActionItem(right);
    action->setText("Paste");
    right->addActionItem(action);
    action = new ActionManager::ActionItem(right);
    action->setText("Copy");
    right->addActionItem(action);
    action = new ActionManager::ActionItem(right);
    action->setText("Cut");
    right->addActionItem(action);

    action = new ActionManager::ActionItem(right);
    action->setText("Right Only!");
    right->addActionItem(action);

    actionManager->registerMenuItem(right);

}


}}
