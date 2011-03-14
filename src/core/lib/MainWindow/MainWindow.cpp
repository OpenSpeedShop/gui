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
#include <PluginManager/PluginDialog.h>

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
MainWindow::MainWindow() : QMainWindow(0)
{
    // Easier notification (than the alternatives) that we're closing
    setAttribute( Qt::WA_DeleteOnClose );

    readSettings();
    initActions();

    PluginManager::PluginManager::instance()->loadPlugins();
}

/*!
   \fn Core::MainWindow::MainWindow()
   \brief Destructor
   \internal
 */
MainWindow::~MainWindow()
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
    SettingManager::SettingManager *settings =
            SettingManager::SettingManager::instance();

    settings->beginGroup("MainWindow");

    resize( settings->value("WindowSize", QSize(640, 400)).toSize() );
    move( settings->value("WindowPosition", QPoint(0, 0)).toPoint() );

    settings->endGroup();
}

/*!
   \fn Core::MainWindow::writeSettings()
   \brief Stores settings in the SettingManager for later retrieval.
   \internal
 */
void MainWindow::writeSettings()
{
    SettingManager::SettingManager *settings =
            SettingManager::SettingManager::instance();

    settings->beginGroup("MainWindow");

    settings->setValue("WindowSize", size());
    settings->setValue("WindowPosition", pos());

    settings->endGroup();
}

void MainWindow::initActions()
{
    ActionManager::ActionManager *actions =
            ActionManager::ActionManager::instance();

    connect(actions, SIGNAL(menuAdded(QMenu *)), this, SLOT(menuAdded(QMenu *)));

    // Create and connect actions to local slots
    QAction *action = new QAction(tr("Plugins"), this);
    action->setStatusTip(tr("View loaded plugins"));
    connect(action, SIGNAL(triggered()), this, SLOT(pluginDialog()));

    // Register actions in menus
    actions->registerMenuItem("Help", action);

}

void MainWindow::menuAdded(QMenu *menu)
{
    menuBar()->addMenu(menu);
}

void MainWindow::pluginDialog()
{
    PluginManager::PluginDialog dialog(this);
    dialog.exec();
}


}}
