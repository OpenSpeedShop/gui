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
MainWindow::MainWindow() :
    QMainWindow(NULL),
    m_Initialized(false)
{
    setWindowIcon(QIcon(":/MainWindow/app.png"));
    initActions();
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

    restoreGeometry(settingManager->value("Geometry").toByteArray());
    restoreState(settingManager->value("State").toByteArray());

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

    settingManager->setValue("Geometry", saveGeometry());
    settingManager->setValue("State", saveState());

    settingManager->endGroup();
}

void MainWindow::initActions()
{
    // Create base (default) menu layout
    QMenu *menu;
    menu = new QMenu(tr("File"), this);
    this->menuBar()->addMenu(menu);
    menu = new QMenu(tr("Edit"), this);
    this->menuBar()->addMenu(menu);
    menu = new QMenu(tr("Tools"), this);
    this->menuBar()->addMenu(menu);
    menu = new QMenu(tr("Window"), this);
    this->menuBar()->addMenu(menu);
    menu = new QMenu(tr("Help"), this);
    this->menuBar()->addMenu(menu);
}


}}
