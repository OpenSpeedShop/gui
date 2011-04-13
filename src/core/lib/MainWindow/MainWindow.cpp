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


#include "MainWindow.h"
#include "ui_MainWindow.h"

namespace Core {
namespace MainWindow {

/*!
   \class Core::MainWindow::MainWindow
   \brief The MainWindow class is part of the core framework libraries. It is
          the actual viewport that the user sees. All plugins manipulate this
          view to expose data to the user.

          singleton class
 */


MainWindow *m_Instance;

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
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_Initialized(false),
  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initActions();
}

/*!
   \fn Core::MainWindow::~MainWindow()
   \brief Destructor
   \internal
 */
MainWindow::~MainWindow()
{
    delete ui;

    if(m_Instance)
        m_Instance = NULL;
}

bool MainWindow::initialize()
{
    readSettings();

    Core::SettingManager::SettingManager *settingManager =
             Core::SettingManager::SettingManager::instance();
    settingManager->registerPageFactory(new MainSettingPageFactory());

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

    QString styleName = settingManager->value("style", QApplication::style()->objectName()).toString();
    QStyle *style = QStyleFactory::create(styleName);
    QApplication::setStyle(style);

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
}

QProgressBar *MainWindow::addProgressBar()
{
    QProgressBar *retval = new QProgressBar(this);
    retval->hide();
    retval->setTextVisible(false);
    retval->setMaximumHeight(15);
    retval->setMaximumWidth(75);
    ui->statusbar->insertWidget(0, retval);
    return retval;
}

void MainWindow::removeProgressBar(QProgressBar *progressBar)
{
    progressBar->setParent(NULL);
    ui->statusbar->removeWidget(progressBar);
}

} // namespace MainWindow
} // namespace Core
