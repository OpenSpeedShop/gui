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

#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QTimer>
#include <QMessageBox>


namespace Plugins {
namespace OpenSpeedShop {

class IConnection;
namespace Ui { class ConnectionWidget; }

class ConnectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionWidget(QWidget *parent = 0);
    ~ConnectionWidget();

protected slots:
    void connectionRegistered(IConnection *);
    void on_btnConnect_clicked();
    void on_btnConnect_toggled(bool checked);
    void on_cmbConnectionType_currentIndexChanged(int index);

    void connectionStateChanged();
    void progress();

protected:
    void startTimeOut(int msec = 3500);
    void stopTimeOut();

    QProgressBar *m_ProgressBar;
    QTimer m_ProgressTimer;
    QMessageBox m_ErrorMessageBox;
    QMessageBox m_TimeoutMessageBox;

private:
    Ui::ConnectionWidget *ui;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // CONNECTIONWIDGET_H