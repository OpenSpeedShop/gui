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

#include <SettingManager/ISettingPage.h>
#include "MainSettingPage.h"

#ifdef MAINWINDOW_DEBUG
#  include <QDebug>
#endif

namespace Core {
namespace MainWindow {

/*!
   \class Core::MainWindow::MainWindow
   \brief The MainWindow class is part of the core framework libraries.
   It is the actual viewport that the user sees. All plugins manipulate this view to expose data to the user.

   Singleton class.

   \interface Core::SettingManager::ISettingPageFactory

 */


/*!
   \fn MainWindow::instance()
   \brief Access to the singleton instance of this class
   \returns A reference to the singleton instance of this class
 */
MainWindow &MainWindow::instance()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::instance";
#endif
    static MainWindow m_Instance;
    return m_Instance;
}

/*!
   \fn MainWindow::MainWindow()
   \brief Constructor.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_Initialized(false)
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::MainWindow";
#endif

    ui->setupUi(this);

    initActions();
}

/*!
   \fn MainWindow::~MainWindow()
   \brief Destructor.
 */
MainWindow::~MainWindow()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::~MainWindow";
#endif

    delete ui;
}

/*!
   \fn MainWindow::initialize()
   \brief Initializes this class after it has been constructed.
   This design pattern allows the class to perform any operations after a class (that this object is dependent upon) has been
   constructed.
   \returns true if successful
   \sa initialized(), shutdown()
 */
bool MainWindow::initialize()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::initialize";
#endif

    try {
        readSettings();

        /* Move the toolbar from the top to the left */
        ui->toolbar->setVisible(true);
        addToolBar(Qt::LeftToolBarArea, ui->toolbar);

        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(this);                         /* Register ourselves as an ISettingPageFactory */
    } catch(...) {
        return false;
    }

    return m_Initialized = true;
}

/*!
   \fn MainWindow::initialized()
   \brief Returns a boolean value indicating whether this instance has been initialized or not.
   \sa initialize()
 */
bool MainWindow::initialized()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::initialized";
#endif

    return m_Initialized;
}

/*!
   \fn MainWindow::shutdown()
   \brief Notifies the instance that it should perform any clean-up operations before destruction.
   This class is called manually, before the application is closed.  It will occur before destruction of the instance.
   \sa initialize()
 */
void MainWindow::shutdown()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::shutdown";
#endif

    writeSettings();
}

/*!
   \fn MainWindow::readSettings()
   \brief Load settings from the SettingManager.
 */
void MainWindow::readSettings()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::readSettings";
#endif

    SettingManager::SettingManager &settingManager = SettingManager::SettingManager::instance();
    settingManager.beginGroup("MainWindow");

    m_StylesheetFilePath = settingManager.value("StylesheetFilePath", QString()).toString();
    QString styleName = settingManager.value("Style", QApplication::style()->objectName()).toString();
    QStyle *style = QStyleFactory::create(styleName);
    QApplication::setStyle(style);

    restoreGeometry(settingManager.value("Geometry").toByteArray());
    restoreState(settingManager.value("State").toByteArray());

    settingManager.endGroup();
}

/*!
   \fn MainWindow::writeSettings()
   \brief Stores settings in the SettingManager for later retrieval.
 */
void MainWindow::writeSettings()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::writeSettings";
#endif

    SettingManager::SettingManager &settingManager = SettingManager::SettingManager::instance();

    settingManager.beginGroup("MainWindow");

    settingManager.setValue("StylesheetFilePath", m_StylesheetFilePath);
    settingManager.setValue("Style", QApplication::style()->objectName());

    settingManager.setValue("Geometry", saveGeometry());
    settingManager.setValue("State", saveState());

    settingManager.endGroup();
}

/*!
   \fn MainWindow::initActions()
   \returns
 */
void MainWindow::initActions()
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::initActions";
#endif
}

/*!
   \fn MainWindow::addProgressBar()
   \returns
 */
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

/*!
   \fn MainWindow::removeProgressBar()
   \returns
 */
void MainWindow::removeProgressBar(QProgressBar *progressBar)
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::removeProgressBar";
#endif

    progressBar->setParent(NULL);
    ui->statusbar->removeWidget(progressBar);
}

/*!
   \fn MainWindow::addCentralWidget()
   This function takes ownership of the supplied widget.
   \returns
 */
void MainWindow::addCentralWidget(QWidget *widget, int priority, QString title, QIcon icon)
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::addCentralWidget";
#endif

    int index = ui->stackedWidget->addWidget(widget);

    if(title.isEmpty()) {
        title = widget->windowTitle();
    }

    if(icon.isNull()) {
        icon = widget->windowIcon();
    }

    QAction *selectWidget = new QAction(icon, title, this);
    selectWidget->setData(index);
    selectWidget->setPriority((QAction::Priority)priority);
    selectWidget->setCheckable(true);
    connect(selectWidget, SIGNAL(triggered()), this, SLOT(setCurrentCentralWidget()));

    bool wasInserted = false;
    foreach(QAction *before, ui->toolbar->actions()) {
        int beforePriority = (int)before->priority();
        if(beforePriority > priority) {
            ui->toolbar->insertAction(before, selectWidget);
            wasInserted = true;
            break;
        }
    }
    if(!wasInserted) {
        ui->toolbar->addAction(selectWidget);
    }

    if(ui->toolbar->actions().count() > 0) {
        ui->toolbar->actions().first()->trigger();
    }
}

/*!
   \fn MainWindow::removeCentralWidget()
   \returns
 */
void MainWindow::removeCentralWidget(QWidget *widget)
{
#ifdef MAINWINDOW_DEBUG
    qDebug() << __FILE__ << __LINE__ << "\tMainWindow::removeCentralWidget";
#endif

    ui->stackedWidget->removeWidget(widget);

    //TODO: remove the widget with icon from the sidebar

}

void MainWindow::setCurrentCentralWidget()
{
    QObject *object = QObject::sender();
    QAction *action = qobject_cast<QAction *>(object);
    if(action) {
        foreach(QAction *action, ui->toolbar->actions()) {
            action->setChecked(false);
        }

        int index = action->data().toInt();
        ui->stackedWidget->setCurrentIndex(index);
        action->setChecked(true);
    }
}



/*!
   \fn MainWindow::notify()
   \returns
 */
NotificationWidget *MainWindow::notify(const QString &text,
                                       NotificationWidget::Icon icon,
                                       NotificationWidget::StandardButtons buttons,
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

/*!
   \fn MainWindow::settingPageIcon()
   \brief Reimplemented from ISettingPageFactory.
   \sa Core::SettingManager::ISettingPageFactory::settingPageIcon()
 */
QIcon MainWindow::settingPageIcon()
{
    return QIcon(":/MainWindow/app.png");
}

/*!
   \fn MainWindow::settingPageName()
   \brief Reimplemented from ISettingPageFactory.
   \reimp Core::SettingManager::ISettingPageFactory::settingPageName()
 */
QString MainWindow::settingPageName()
{
    return tr("Core");
}

/*!
   \fn MainWindow::settingPagePriority()
   \brief Reimplemented from ISettingPageFactory.
   \reimp Core::SettingManager::ISettingPageFactory::settingPagePriority()
 */
int MainWindow::settingPagePriority()
{
    return 10;
}

/*!
   \fn MainWindow::createSettingPage()
   \brief Reimplemented from ISettingPageFactory.
   \reimp Core::SettingManager::ISettingPageFactory::createSettingPage()
 */
SettingManager::ISettingPage *MainWindow::createSettingPage()
{
    return new MainSettingPage();
}


} // namespace MainWindow
} // namespace Core
