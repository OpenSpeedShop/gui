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

#ifndef ICONNECTIONPAGE_H
#define ICONNECTIONPAGE_H

#include <QtCore>
#include <QtGui>

#include "ConnectionManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class CONNECTIONMANAGER_EXPORT IConnectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit IConnectionPage(QWidget *parent = 0) : QWidget(parent) {}
    virtual ~IConnectionPage() {}

public slots:
    virtual void apply() = 0;
    virtual void reset() = 0;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#ifndef STRINGIFY
#define STRINGIFY(X) # X
#endif

#define ICONNECTIONPAGE_VERSION "org.openspeedshop.gui.IConnectionPage/" STRINGIFY(VER_MAJ) "." STRINGIFY(VER_MIN)
Q_DECLARE_INTERFACE(Plugins::OpenSpeedShop::IConnectionPage, ICONNECTIONPAGE_VERSION)

#endif // ICONNECTIONPAGE_H
