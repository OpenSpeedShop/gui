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

#ifndef IVIEWPLUGIN_H
#define IVIEWPLUGIN_H

#include <QtCore>
#include <QtGui>

#include "ViewManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class VIEWMANAGER_EXPORT IViewFactory
{
public:
    virtual QString viewName() = 0;
    virtual bool viewHandles(QAbstractItemModel *model) = 0;
    virtual QAbstractItemView *viewWidget(QAbstractItemModel *model) = 0;
};

} // namespace OpenSpeedShop
} // namespace Plugins

Q_DECLARE_INTERFACE(Plugins::OpenSpeedShop::IViewFactory, "org.openspeedshop.gui.IViewFactory/0.1")

#endif // IVIEWPLUGIN_H
