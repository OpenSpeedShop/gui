/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2013 Argo Navis Technologies, LLC

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

#include <SettingManager/SettingManager.h>

#include "ConnectionManager.h"
#include "IConnection.h"
#include "IConnectionPage.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \class ConnectionWidget
    \brief Widget that displays connection data and settings to the user in a
           dock panel.
    \sa ConnectionManager
 */

ConnectionWidget::ConnectionWidget(QWidget *parent) :
    Core::SettingManager::ISettingPage(parent),
    ui(new Ui::ConnectionWidget)
{
    ui->setupUi(this);

    // Grab any connections that already exist and register for notification of any new ones that are registered
    ConnectionManager &connectionManager = ConnectionManager::instance();
    foreach(IConnection *connection, connectionManager.m_Connections) { connectionRegistered(connection); }
    connect(&connectionManager, SIGNAL(connectionRegistered(IConnection*)), this, SLOT(connectionRegistered(IConnection*)));

    initialize();
}

ConnectionWidget::~ConnectionWidget()
{
    delete ui;
}

void ConnectionWidget::connectionRegistered(IConnection *connection)
{
    QWidget *page = connection->page();
    ui->stackedWidget->addWidget(page);     /* This takes ownership of the page, and is responsible for deleting it from memory */
    ui->cmbConnectionType->addItem(page->windowIcon(), connection->name(), qVariantFromValue(connection));
}

void ConnectionWidget::on_cmbConnectionType_currentIndexChanged(int index)
{
    // Simply return if nothing changed
    static int oldIndex = -1;
    if(index == oldIndex)
        return;

    if(oldIndex >= 0) {
        IConnection *oldConnection = ui->cmbConnectionType->itemData(oldIndex).value<IConnection *>();

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
            } else {
                // User said, "no." Put it back the way it was
                ui->cmbConnectionType->setCurrentIndex(oldIndex);
                return;
            }
        }
    }

    // Set up the new connection
    ui->stackedWidget->setCurrentIndex(index);
    IConnection *newConnection = ui->cmbConnectionType->itemData(index).value<IConnection *>();
    ConnectionManager::instance().setCurrentConnection(newConnection);
    oldIndex = index;
}

void ConnectionWidget::initialize()
{
    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/ConnectionManager");

    QString connectionName = settingManager.value("ConnectionName", ui->cmbConnectionType->itemText(0)).toString();
    int index = ui->cmbConnectionType->findText(connectionName, Qt::MatchExactly);
    ui->cmbConnectionType->setCurrentIndex(index);

    settingManager.endGroup();
}

void ConnectionWidget::apply()
{
    // Persist changed settings to SettingManager
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/ConnectionManager");
    settingManager.setValue("ConnectionName", ui->cmbConnectionType->currentText());
    settingManager.endGroup();

    for(int i=0; i < ui->stackedWidget->count(); i++) {
        IConnectionPage *page = qobject_cast<IConnectionPage *>(ui->stackedWidget->widget(i));
        if(page) {
            page->apply();
        }
    }

    // Ask the conncetion manager to refresh it's settings
    ConnectionManager::instance().readSettings();
}

void ConnectionWidget::reset()
{
    for(int i=0; i < ui->stackedWidget->count(); i++) {
        IConnectionPage *page = qobject_cast<IConnectionPage *>(ui->stackedWidget->widget(i));
        if(page) {
            page->reset();
        }
    }

    initialize();
}



} // namespace OpenSpeedShop
} // namespace Plugins
