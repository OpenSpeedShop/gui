/*!
   \file MainWindow.h
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QProgressBar>

#include <SettingManager/SettingManager.h>
#include <SettingManager/ISettingPageFactory.h>
#include <PluginManager/PluginManager.h>

#include "MainWindowLibrary.h"
#include "NotificationWidget.h"

namespace Core {
namespace MainWindow {

namespace Ui {
    class MainWindow;
}

class MAINWINDOW_EXPORT MainWindow : public QMainWindow, public SettingManager::ISettingPageFactory
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPageFactory)

public:
    static MainWindow &instance();

    bool initialize();
    bool initialized();
    void shutdown();

    QProgressBar *addProgressBar();
    void removeProgressBar(QProgressBar *progressBar);

    void addCentralWidget(QWidget *widget, int priority = 128, QString title = QString(), QIcon icon = QIcon());
    void removeCentralWidget(QWidget *widget);

    NotificationWidget *notify(const QString &text,
                NotificationWidget::Icon icon = NotificationWidget::NoIcon,
                NotificationWidget::StandardButtons buttons = NotificationWidget::NoButton,
                const QObject *reciever = NULL, const char *member = NULL);

    /* BEGIN ISettingPageFactory */
    QIcon settingPageIcon();
    QString settingPageName();
    int settingPagePriority();
    SettingManager::ISettingPage *createSettingPage();
    /* END ISettingPageFactory */


protected:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void readSettings();
    void writeSettings();
    void initActions();

protected slots:
    void setCurrentCentralWidget();


private:
    Ui::MainWindow *ui;
    bool m_Initialized;
    QString m_StylesheetFilePath;

    friend class MainSettingPage;
};


} // namespace MainWindow
} // namespace Core
#endif // MAINWINDOW_H
