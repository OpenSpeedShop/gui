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

#ifndef IVIEWFILTERABLE_H
#define IVIEWFILTERABLE_H


#include <QtCore>
#include <QtPlugin>

#include "ViewManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class VIEWMANAGER_EXPORT IViewFilterable
{
public:
    virtual QString viewFilter() const = 0;
    virtual void setViewFilter(const QString &regex) = 0;
    virtual int viewFilterColumn() const = 0;
    virtual void setViewFilterColumn(int column = 0) = 0;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#ifndef STRINGIFY
#define STRINGIFY(X) # X
#endif

#define IVIEWFILTERABLE_VERSION "org.openspeedshop.gui.IViewFilterable/" STRINGIFY(VER_MAJ) "." STRINGIFY(VER_MIN)
Q_DECLARE_INTERFACE(Plugins::OpenSpeedShop::IViewFilterable, IVIEWFILTERABLE_VERSION)

#endif // IVIEWFILTERABLE_H
