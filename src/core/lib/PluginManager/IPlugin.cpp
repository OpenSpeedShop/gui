/**
 * @file IPlugin.cpp
 * @author Dane Gardner <dane.gardner@gmail.com>
 * @version
 *
 * @section LICENSE
 * This file is part of the Open|SpeedShop Graphical User Interface
 * Copyright (C) 2010-2011 Argo Navis Technologies, LLC
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @section DESCRIPTION
 *
 */

#include "IPlugin.h"

/**
 * @class IPlugin
 * @brief to be overriden as an interface for plugins
 *
 * pure virtual class
 */

/**
 * @fn IPlugin::IPlugin()
 * @brief Constructor
 */
IPlugin::IPlugin()
{

}

/**
 * @fn IPlugin::~IPlugin()
 * @brief Destructor
 */

/**
 * @fn IPlugin::initialize()
 * @brief This method is called after the plugin has been instantiated and
 *        after the plugin's dependencies have been initialized.
 * @param args QStringList containing required arguments
 * @param err Returned errors in human-readable form
 * @returns true if successful; false otherwise
 */

/** @fn IPlugin::shutdown()
 * @brief This method is called in the reverse order of the initialize
 *        method and notifies the plugin that we are about to shutdown
 *        --before it is destroyed.
 */
