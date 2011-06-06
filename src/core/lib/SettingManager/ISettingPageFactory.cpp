/*!
   \file ISettingPageFactory.cpp
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

#include "ISettingPageFactory.h"

namespace Core {
namespace SettingManager {

/*! \interface Core::SettingManager::ISettingPageFactory
    \brief Interface for setting page factories.
    This allows a setting page to be registered with the SettingManager and displayed to the user in the settings dialog.
    Pure virtual class.
 */

/*! \fn ISettingPageFactory::settingPageIcon()
    \brief Returns the desired icon for the setting page.
 */

/*! \fn ISettingPageFactory::settingPageName()
    \brief Returns the desired name of the setting page.
 */

/*! \fn ISettingPageFactory::settingPagePriority()
    \brief Returns the priority level of the setting page.
    The priority determines the display order for the setting page.  The lower the number, the closer to the upper-left
    corner it will be displayed.  The higher the number, the closer to the lower-right (or farther from the upper-left,
    depending on how one looks at it) the page information will be displayed.
 */

/*! \fn ISettingPageFactory::createSettingPage()
    \brief The widget that will be displayed within a frame in the settings dialog area.
 */

} // namespace SettingManager
} // namespace Core
