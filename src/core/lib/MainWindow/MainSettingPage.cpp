/*!
   \file MainSettingPage.cpp
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

#include "MainSettingPage.h"
#include "ui_MainSettingPage.h"

#include <QDebug>

namespace Core {
namespace MainWindow {

MainSettingPage::MainSettingPage(QWidget *parent) :
    SettingManager::ISettingPage(parent),
    ui(new Ui::MainSettingPage)
{
    ui->setupUi(this);

    m_OriginalStyle = QApplication::style()->objectName();
    foreach(QString style, QStyleFactory::keys()) {
        ui->cmbStyle->addItem(style);
    }

    ui->cmbToolbarIcons->insertItem(0, "Large", 32);
    ui->cmbToolbarIcons->insertItem(1, "Medium", 16);
    ui->cmbToolbarIcons->insertItem(2, "Small", 12);
    ui->cmbToolbarIcons->setCurrentIndex(1);

    reset();
}

MainSettingPage::~MainSettingPage()
{
    // Reset the style if necessary
    if(QApplication::style()->objectName().compare(m_OriginalStyle, Qt::CaseInsensitive))
        QApplication::setStyle(QStyleFactory::create(m_OriginalStyle));

    delete ui;
}

void MainSettingPage::apply()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("MainWindow");

    m_OriginalStyle = QApplication::style()->objectName();
    settingManager->setValue("style", m_OriginalStyle);

    settingManager->endGroup();
}

void MainSettingPage::reset()
{
    SettingManager::SettingManager *settingManager =
            SettingManager::SettingManager::instance();

    settingManager->beginGroup("MainWindow");

    QString style = settingManager->value("style", m_OriginalStyle).toString();
    int index = ui->cmbStyle->findText(style, Qt::MatchFixedString);
    ui->cmbStyle->setCurrentIndex(index);

    settingManager->endGroup();
}

void MainSettingPage::on_cmbStyle_currentIndexChanged(QString style)
{
    // Ensure we're actually changing the style
    if(!QApplication::style()->objectName().compare(ui->cmbStyle->currentText(), Qt::CaseInsensitive))
        return;

    // Set the style to the new, selected one
    QApplication::setStyle(QStyleFactory::create(style));
}


} // namespace MainWindow
} // namespace Core
