/**
 * @file MainWindow.cpp
 * @author Dane Gardner <dane.gardner@gmail.com>
 * @version
 *
 * @section LICENSE
 * This file is part of the Open|SpeedShop Graphical User Interface
 * Copyright (C) 2010-2011 Argo Navis Technologies, LLC
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @section DESCRIPTION
 *
 */

#include "MainWindow.h"

namespace Core {

MainWindow *m_Instance;

/**
 * @class MainWindow
 * @mainclass
 *
 * @brief The MainWindow class is part of the core framework libraries. It is
 *        the actual viewport that the user sees. All plugins manipulate this
 *        view to expose data to the user.
 *
 *        singleton class
 */

/**
 * @fn MainWindow::instance()
 * @brief Access to the singleton instance of this class
 * @returns A pointer to the singleton instance of this class
 */
MainWindow *MainWindow::instance()
{
    return m_Instance ? m_Instance : m_Instance = new MainWindow();
}

/**
 * @fn MainWindow::MainWindow()
 * @brief Constructor
 * @internal
 */
MainWindow::MainWindow() : QMainWindow(0)
{
    // Easier notification (than the alternatives) that we're closing
    setAttribute( Qt::WA_DeleteOnClose );

    readSettings();
    initActions();

    PluginManager::instance()->loadPlugins();
}

/**
 * @fn MainWindow::MainWindow()
 * @brief Destructor
 * @internal
 */
MainWindow::~MainWindow()
{
    writeSettings();
}

/**
 * @fn MainWindow::readSettings()
 * @brief Load settings from the SettingManager.
 * @internal
 */
void MainWindow::readSettings()
{
    SettingManager *settings = SettingManager::instance();
    settings->beginGroup("MainWindow");

    resize( settings->value("WindowSize", QSize(640, 400)).toSize() );
    move( settings->value("WindowPosition", QPoint(0, 0)).toPoint() );

    settings->endGroup();
}

/**
 * @fn MainWindow::writeSettings()
 * @brief Stores settings in the SettingManager for later retrieval.
 * @internal
 */
void MainWindow::writeSettings()
{
    SettingManager *settings = SettingManager::instance();
    settings->beginGroup("MainWindow");

    settings->setValue("WindowSize", size());
    settings->setValue("WindowPosition", pos());

    settings->endGroup();
}

void MainWindow::initActions()
{
    ActionManager *actions = ActionManager::instance();
    connect(actions, SIGNAL(menuAdded(QMenu *)), this, SLOT(menuAdded(QMenu *)));

    // Create and connect actions to local slots
    QAction *action = new QAction(tr("&New"), this);
    action->setShortcut(QKeySequence::New);
    action->setStatusTip(tr("Create a new file"));
    connect(action, SIGNAL(triggered()), this, SLOT(fileNew()));

    // Register actions in menus
    actions->registerMenuItem("File", action);

}

void MainWindow::menuAdded(QMenu *menu)
{
    menuBar()->addMenu(menu);
}

void MainWindow::fileNew()
{
    qDebug("fileNew() triggered");
}

}
