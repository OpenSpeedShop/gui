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

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \class AboutDialog
    \brief Handles the displaying of the about and splash pages for the
           Open|SpeedShop plugin set.
    \sa OpenSpeedShopPlugin
 */

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString aboutText;
    aboutText.append(tr("%1 v%2\n").
                     arg(QCoreApplication::applicationName()).
                     arg(QCoreApplication::applicationVersion()));
    aboutText.append(tr("Built on %1 at %2\n").arg(__DATE__).arg(__TIME__));
    aboutText.append(tr("Copyright 2010-2011 Argo Navis Technologies LLC.  All rights reserved.\n"));
    ui->lblAbout->setText(aboutText);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::splash(int ms)
{
    AboutDialog *aboutDialog = new AboutDialog();
    aboutDialog->ui->buttonBox->setHidden(true);
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose);    // So we don't have to worry about deleting it
    aboutDialog->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
    aboutDialog->setModal(true);
    aboutDialog->layout()->setMargin(0);

    aboutDialog->open();
    QTimer::singleShot(ms, aboutDialog, SLOT(close()));
}

} // namespace OpenSpeedShop
} // namespace Plugins
