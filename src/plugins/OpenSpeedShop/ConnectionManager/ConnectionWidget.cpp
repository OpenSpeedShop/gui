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

#include "ConnectionWidget.h"
#include "ui_ConnectionWidget.h"

#include <MainWindow/MainWindow.h>
#include "ConnectionManager.h"
#include "IConnection.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \class ConnectionWidget
    \brief Widget that displays connection data and settings to the user in a
           dock panel.
    \sa ConnectionManager
 */

ConnectionWidget::ConnectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionWidget)
{
    ui->setupUi(this);

    m_ProgressBar = Core::MainWindow::MainWindow::instance()->addProgressBar();
    connect(&m_ProgressTimer, SIGNAL(timeout()), this, SLOT(progress()));

    ConnectionManager *connectionManager = ConnectionManager::instance();

    // Grab any connections that already exist
    foreach(IConnection *connection, connectionManager->m_Connections)
        connectionRegistered(connection);

    // Register for notification of any new ones that are registered
    connect(connectionManager, SIGNAL(connectionRegistered(IConnection*)),
            this, SLOT(connectionRegistered(IConnection*)));
}

ConnectionWidget::~ConnectionWidget()
{
    delete ui;
}

void ConnectionWidget::connectionRegistered(IConnection *connection)
{
    QWidget *page = connection->page();
    ui->stackedWidget->addWidget(page);
    ui->cmbConnectionType->addItem(page->windowIcon(),
                                   page->windowTitle(),
                                   qVariantFromValue(connection));
}

void ConnectionWidget::on_btnConnect_clicked()
{
    if(ui->btnConnect->isChecked()) {
        int index = ui->cmbConnectionType->currentIndex();
        IConnection *connection = ui->cmbConnectionType->itemData(index).value<IConnection *>();

        if(connection->state() == IConnection::State_Disconnected)
            connection->connectToServer();
    } else {
        int index = ui->cmbConnectionType->currentIndex();
        IConnection *connection = ui->cmbConnectionType->itemData(index).value<IConnection *>();

        if(connection->state() == IConnection::State_Connected)
            connection->disconnectFromServer();
    }
}

void ConnectionWidget::on_btnConnect_toggled(bool checked)
{
    QString text = checked ? tr("Disconnect") : tr("Connect");
    ui->btnConnect->setToolTip(text);
    ui->btnConnect->setStatusTip(text);
}

void ConnectionWidget::on_cmbConnectionType_currentIndexChanged(int index)
{
    // Simply return if nothing changed
    static int oldIndex = -1;
    if(index == oldIndex)
        return;

    if(oldIndex >= 0) {
        IConnection *oldConnection =
                ui->cmbConnectionType->itemData(oldIndex).value<IConnection *>();

        // Check the state of the current connection
        if(oldConnection->state() != IConnection::State_Disconnected) {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);
            msg.setWindowTitle(tr("Warning!"));
            msg.setText(tr("This action will disconnect you from the currently connected "
                           "connection. Do you wish to continue?"));
            msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

            if(msg.exec() == QMessageBox::Yes) {
                oldConnection->abort();
                stopTimeOut();
            } else {
                // User said, "no." Put it back the way it was
                ui->cmbConnectionType->setCurrentIndex(oldIndex);
                return;
            }
        }

        // Disconnect the old one
        disconnect(oldConnection, SIGNAL(stateChanged()),
                   this, SLOT(connectionStateChanged()));
    }

    // Set up the new connection
    ui->stackedWidget->setCurrentIndex(index);
    IConnection *newConnection =
            ui->cmbConnectionType->itemData(index).value<IConnection *>();
    connect(newConnection, SIGNAL(stateChanged()),
            this, SLOT(connectionStateChanged()));

    ConnectionManager::instance()->setCurrentConnection(newConnection);

    oldIndex = index;
}

void ConnectionWidget::startTimeOut(int msec)
{
    m_ProgressTimer.start(msec/100);
}

void ConnectionWidget::stopTimeOut()
{
    m_ProgressTimer.stop();
    m_ProgressBar->hide();
    m_ProgressBar->reset();
}

void ConnectionWidget::progress()
{
    if(m_ProgressBar->value() < m_ProgressBar->maximum()) {
        m_ProgressBar->show();
        m_ProgressBar->setValue(m_ProgressBar->value() + 1);
        return;
    }

    m_ProgressTimer.stop();
    m_ProgressBar->hide();
    m_ProgressBar->reset();

    if(m_ErrorMessageBox.isVisible())
        return;

    m_TimeoutMessageBox.setIcon(QMessageBox::Warning);
    m_TimeoutMessageBox.setWindowTitle(tr("Connection time out"));
    m_TimeoutMessageBox.setText(tr("A connection is taking longer than expected to perform an operation. Would you like to continue waiting?"));
    m_TimeoutMessageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    if(m_TimeoutMessageBox.exec() == QMessageBox::Yes) {
        startTimeOut(10000);
        return;
    }

    //! \todo Deal with stopping the connection/disconnection thread
}

void ConnectionWidget::connectionStateChanged()
{
    IConnection *connection = qobject_cast<IConnection *>(QObject::sender());

    if(!connection)
        throw new QString("Caught signal from unexpected object");

    switch(connection->state()) {
    case IConnection::State_Connecting:
        ui->btnConnect->setEnabled(false);
        ui->btnConnect->setChecked(true);
        startTimeOut();
        break;
    case IConnection::State_Connected:
        ui->btnConnect->setEnabled(true);
        ui->btnConnect->setChecked(true);
        stopTimeOut();
        break;
    case IConnection::State_Disconnecting:
        ui->btnConnect->setEnabled(false);
        ui->btnConnect->setChecked(false);
        startTimeOut();
        break;
    case IConnection::State_Disconnected:
        ui->btnConnect->setEnabled(true);
        ui->btnConnect->setChecked(false);
        stopTimeOut();
        break;
    case IConnection::State_Error:
        ui->btnConnect->setEnabled(false);

        stopTimeOut();
        if(m_TimeoutMessageBox.isVisible()) {
            m_TimeoutMessageBox.close();
        }

        m_ErrorMessageBox.setIcon(QMessageBox::Critical);
        m_ErrorMessageBox.setWindowTitle(tr("Connection Error"));
        m_ErrorMessageBox.setText(connection->errorMessage());
        m_ErrorMessageBox.setStandardButtons(QMessageBox::Ok);
        m_ErrorMessageBox.exec();

        // Deal with stopping the connection/disconnection thread
        int index = ui->cmbConnectionType->currentIndex();
        IConnection *connection =
                ui->cmbConnectionType->itemData(index).value<IConnection *>();
        connection->abort();

        break;
    }
}


} // namespace OpenSpeedShop
} // namespace Plugins
