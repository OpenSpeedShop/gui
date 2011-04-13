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

#ifndef DIRECTCONNECTIONPAGE_H
#define DIRECTCONNECTIONPAGE_H

#include <QWidget>
#include "DirectConnection.h"

namespace Plugins {
namespace ConnectionManager {

namespace Ui { class DirectConnectionPage; }
class DirectConnection;

class DirectConnectionPage : public QWidget
{
    Q_OBJECT

public:
    explicit DirectConnectionPage(DirectConnection *parentConnection, QWidget *parent = 0);
    ~DirectConnectionPage();

protected slots:
    void on_txtHostname_textChanged();
    void on_txtPort_textChanged();
    void on_btnSetDefault_clicked();

protected:
    DirectConnection *m_ParentConnection;

private:
    Ui::DirectConnectionPage *ui;
};


} // namespace OpenSpeedShop
} // namespace Plugins

#endif // DIRECTCONNECTIONPAGE_H
