/*!
   \file PluginManager.cpp
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

#include <QDir>
#include <QPluginLoader>
#include "PluginManager.h"

namespace Core {

/*!
   \class PluginManager
   \brief The PluginManager class is part of the core framework libraries, and
          manages the loading of and interaction between dynamic extensions
          and plugins.  This includes managing an "object pool."

          singleton class
 */

/*!
   \fn T *PluginManager::getObject() const
   \returns The given object type that has been stored in the manager.
 */

PluginManager *m_Instance;

/*!
   \fn PluginManager::instance()
   \brief Access to the singleton instance of this class
   \returns A pointer to the singleton instance of this class
 */
PluginManager *PluginManager::instance()
{
    return m_Instance ? m_Instance : m_Instance = new PluginManager();
}

/*!
   \fn PluginManager::PluginManager()
   \brief Constructor
   \internal
 */
PluginManager::PluginManager() : QObject(0)
{
    readSettings();
}

/*!
   \fn PluginManager::PluginManager()
   \brief Destructor
   \internal
 */
PluginManager::~PluginManager()
{
    //Should we be doing this in the reverse order?
    foreach (QObject *obj, m_Objects)
        delObject(obj);

    writeSettings();
}

/*!
   \fn PluginManager::readSettings()
   \brief Load settings from the SettingManager.
   \internal
 */
void PluginManager::readSettings()
{
    SettingManager *settings = SettingManager::instance();
    settings->beginGroup("PluginManager");

    m_PluginPath = settings->value("PluginPath").toString();

    settings->endGroup();
}

/*!
   \fn PluginManager::writeSettings()
   \brief Stores settings in the SettingManager for later retrieval.
   \internal
 */
void PluginManager::writeSettings()
{
    SettingManager *settings = SettingManager::instance();
    settings->beginGroup("PluginManager");

    settings->setValue("PluginPath", m_PluginPath);

    settings->endGroup();
}

/*!
   \fn PluginManager::loadPlugins()
   \brief Loads plugins from a location defined in the SettingManager

   Emits pluginLoaded() signal after loading \b each plugin
 */
void PluginManager::loadPlugins()
{
    //TODO: Get the directory from the SettingManager
    QDir pluginDir("C:/Users/Dane/OpenSpeedShop-build/lib/plugins");

    foreach (QString fileName, pluginDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            IPlugin *interface = qobject_cast<IPlugin *>(plugin);
            if (interface) {
                m_Plugins.append(interface);
                emit pluginLoaded(interface);
            }
        }
    }
}

/*!
   \fn PluginManager::addObject()
   \brief Adds an object to the manager for later retrieval. This is typically
          used by plugins as they are initialized to store factory classes.

   Emits objectAdding() signal before adding
   Emits objectAdded() signal after adding
   \param object The object to be stored
 */
void PluginManager::addObject(QObject *object)
{
    emit objectAdding(object);
    m_Objects.append(object);
    emit objectAdded(object);
}

/*!
   \fn PluginManager::delObject()
   \brief Removes a previously stored object from the manager.

   Emits objectRemoving() signal before removal
   Emits objectRemoved() signal after removal
   \param object The object to be removed
 */
bool PluginManager::delObject(QObject *object)
{
    emit objectRemoving(object);
    int RetVal;
    RetVal = m_Objects.removeAll(object);
    emit objectRemoved(object);
    return (RetVal);
}

}
