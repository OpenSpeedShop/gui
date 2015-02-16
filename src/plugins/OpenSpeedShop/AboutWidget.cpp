/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2015 Argo Navis Technologies, LLC

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

#include "AboutWidget.h"
#include "ui_AboutWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \class AboutWidget
    \brief Handles the displaying of the about page for the Open|SpeedShop plugin set.
    \sa OpenSpeedShopPlugin
 */

AboutWidget::AboutWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWidget)
{
    ui->setupUi(this);

    QString aboutText;
    aboutText.append(tr("%1 v%2\n").
                     arg(QCoreApplication::applicationName()).
                     arg(QCoreApplication::applicationVersion()));
    aboutText.append(tr("Built on %1 at %2\n").arg(__DATE__).arg(__TIME__));
    aboutText.append(tr("Copyright 2010-2015 Argo Navis Technologies LLC.  All rights reserved.\n"));
    ui->lblAbout->setText(aboutText);
}

AboutWidget::~AboutWidget()
{
    delete ui;
}

} // namespace OpenSpeedShop
} // namespace Plugins
