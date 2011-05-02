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
#include <PluginManager/PluginManager.h>

#include "MainSettingPageFactory.h"
#include "NotificationWidget.h"

namespace Core {
namespace MainWindow {

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow* instance();

    ~MainWindow();
    bool initialize();
    bool initialized();
    void shutdown();

    QProgressBar *addProgressBar();
    void removeProgressBar(QProgressBar *progressBar);

    void setCentralWidget(QWidget *widget);

    void notify(const QString &text,
                NotificationWidget::Icon icon = NotificationWidget::NoIcon,
                NotificationWidget::StandardButtons buttons = NotificationWidget::NoButton,
                const QObject *reciever = NULL, const char *member = NULL);

protected:
    explicit MainWindow(QWidget *parent = 0);

    void readSettings();
    void writeSettings();
    void initActions();

    bool m_Initialized;
    QString m_StylesheetFilePath;

private:
    Ui::MainWindow *ui;

    friend class MainSettingPage;
};


} // namespace MainWindow
} // namespace Core
#endif // MAINWINDOW_H
