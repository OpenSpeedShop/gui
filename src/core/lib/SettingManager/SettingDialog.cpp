/*!
   \file SettingDialog.cpp
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

#include "SettingDialog.h"
#include "ui_SettingDialog.h"

namespace Core {
namespace SettingManager {

SettingDialog::SettingDialog(QList<ISettingPageFactory *> pages, QWidget *parent) :
    QDialog(parent),
    m_Pages(pages),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    readSettings();

    // Connect the listWidget selections with the displayed page
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)),
            ui->stackedWidget, SLOT(setCurrentIndex(int)));

    // Honor priority in the page listing
    qSort(m_Pages.begin(), m_Pages.end(), ascending);

    // Clear previous pages
    while(ui->stackedWidget->count()) {
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    }

    // Add the pages to the lists, after initializing them
    for(int i = 0; i < m_Pages.count(); i++) {
        ISettingPageFactory *factory = m_Pages.at(i);

        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setText(factory->name());
        item->setIcon(factory->icon());

        ISettingPage *page = factory->createPage();
        page->initialize();
        ui->stackedWidget->addWidget( qobject_cast<QWidget *>(page) );
    }
}

SettingDialog::~SettingDialog()
{
    writeSettings();
    delete ui;
}

void SettingDialog::readSettings()
{
    SettingManager::SettingManager *settings =
            SettingManager::SettingManager::instance();

    settings->beginGroup("SettingManager");
    settings->beginGroup("SettingDialog");

    resize( settings->value("WindowSize", size()).toSize() );
    move( settings->value("WindowPosition", pos()).toPoint() );

    settings->endGroup();
    settings->endGroup();
}

void SettingDialog::writeSettings()
{
    SettingManager::SettingManager *settings =
            SettingManager::SettingManager::instance();

    settings->beginGroup("SettingManager");
    settings->beginGroup("SettingDialog");

    settings->setValue("WindowSize", size());
    settings->setValue("WindowPosition", pos());

    settings->endGroup();
    settings->endGroup();
}

bool SettingDialog::ascending(ISettingPageFactory *left, ISettingPageFactory *right)
{
    return left->priority() < right->priority();
}

void SettingDialog::accept()
{
    // Iterate over the pages and persist the changes to the settings
    for(int i = 0; i < ui->stackedWidget->count(); i++) {
        ISettingPage *page = qobject_cast<ISettingPage *>(ui->stackedWidget->widget(i));
        if(page) {
            page->apply();
        }
    }

    QDialog::accept();
}

void SettingDialog::reject()
{
    QDialog::reject();
}

}}
