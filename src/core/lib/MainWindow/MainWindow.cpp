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

#ifdef MAINWINDOW_DEBUG
#  include <QDebug>
#endif

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
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::instance";
#endif

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
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::MainWindow";
#endif

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
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::~MainWindow";
#endif

    delete ui;

    if(m_Instance)
        m_Instance = NULL;
}

bool MainWindow::initialize()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::initialize";
#endif

    readSettings();

    Core::SettingManager::SettingManager *settingManager =
             Core::SettingManager::SettingManager::instance();
    settingManager->registerPageFactory(new MainSettingPageFactory());

    return m_Initialized = true;
}

bool MainWindow::initialized()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::initialized";
#endif

    return m_Initialized;
}

void MainWindow::shutdown()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::shutdown";
#endif

    writeSettings();
}

/*!
   \fn Core::MainWindow::readSettings()
   \brief Load settings from the SettingManager.
   \internal
 */
void MainWindow::readSettings()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::readSettings";
#endif

    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("MainWindow");

    m_StylesheetFilePath = settingManager->value("StylesheetFilePath", QString()).toString();
    QString styleName = settingManager->value("Style", QApplication::style()->objectName()).toString();
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
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::writeSettings";
#endif

    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("MainWindow");

    settingManager->setValue("StylesheetFilePath", m_StylesheetFilePath);
    settingManager->setValue("Style", QApplication::style()->objectName());

    settingManager->setValue("Geometry", saveGeometry());
    settingManager->setValue("State", saveState());

    settingManager->endGroup();
}

void MainWindow::initActions()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::initActions";
#endif
}

QProgressBar *MainWindow::addProgressBar()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::addProgressBar";
#endif

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
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::removeProgressBar";
#endif

    progressBar->setParent(NULL);
    ui->statusbar->removeWidget(progressBar);
}

void MainWindow::setCentralWidget(QWidget *widget)
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::setCentralWidget";
#endif

    QWidget *oldWidget = ui->placehoderWidget;
    ui->centralLayout->removeWidget(oldWidget);
    delete oldWidget;

    widget->setParent(this);
    ui->placehoderWidget = widget;
    ui->centralLayout->addWidget(widget);
}

NotificationWidget *MainWindow::notify(const QString &text, NotificationWidget::Icon icon, NotificationWidget::StandardButtons buttons,
            const QObject *reciever, const char *member)
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::notify";
#endif

    NotificationWidget *notificationWidget = new NotificationWidget(text, icon, buttons, reciever, member, this);
    ui->centralLayout->insertWidget(0, notificationWidget);
    notificationWidget->setFocus();

    return notificationWidget;
}


} // namespace MainWindow
} // namespace Core
