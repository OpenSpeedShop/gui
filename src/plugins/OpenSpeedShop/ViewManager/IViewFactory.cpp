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

#include "IViewFactory.h"

namespace Plugins {
namespace OpenSpeedShop {

/*! \fn handles()
    \brief Returns true if the view can handle the supplied model.
    \param model Is a pointer to the model that the caller wishes to handle.
    \returns true if the model can be handled; otherwise returns false.
    \exceptions None.
    \sa view()
 */

/*! \fn view()
    \brief Factory function to create a pointer to a new view referencing the supplied model.
    \note The view is not memory managed by this plugin, and must be handled by the caller.
    \param model Is a pointer to the model that the caller wishes the returned view to handle.
    \returns NULL if the model cannot be handled by this view.
    \sa handles()
    \exceptions None.
 */

} // namespace OpenSpeedShop
} // namespace Plugins
