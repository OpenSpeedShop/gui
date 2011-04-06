/*!
   \file SettingManager.cpp
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

#include "SettingManager.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

namespace Core {
namespace SettingManager {

/*!
   \class SettingManager
   \brief Manages persisted settings throughout the framework.
   singleton class

   \note Right now, this is just a wrapper for the QSettings object that comes
         with Qt4. In the future, I'd like to see something that is a bit more
         robust backing the settings in this project.
 */

SettingManager *m_Instance;

/*!
   \fn SettingManager::instance()
   \brief Access to the singleton instance of this class
   \returns A pointer to the singleton instance of this class
 */
SettingManager *SettingManager::instance()
{
    return m_Instance ? m_Instance : m_Instance = new SettingManager();
}

/*!
   \fn SettingManager::SettingManager()
   \brief Constructor
   \internal
 */
SettingManager::SettingManager() : QObject(0)
{
    m_Initialized = false;
}

/*!
   \fn SettingManager::~SettingManager()
   \brief Destructor
   \internal
 */
SettingManager::~SettingManager()
{
    m_Settings.sync();

    while(!m_Pages.isEmpty())
        delete m_Pages.takeFirst();

    if(m_Instance)
        m_Instance = NULL;
}

bool SettingManager::initialize()
{
    MainWindow::MainWindow *mainWindow = MainWindow::MainWindow::instance();
    foreach(QAction *action, mainWindow->menuBar()->actions()) {
        if(action->text() == tr("Tools")) {
            QAction *settingDialog = new QAction(tr("Settings"), this);
            settingDialog->setToolTip(tr("Change application and plugin settings"));
            connect(settingDialog, SIGNAL(triggered()), this, SLOT(settingDialog()));
            action->menu()->addAction(settingDialog);
        }
    }

    return m_Initialized = true;
}

bool SettingManager::initialized()
{
    return m_Initialized;
}

void SettingManager::shutdown()
{
}

/*!
   \fn SettingManager::setValue()
   \brief Sets the value of setting key to value. If the key already exists, the previous value is overwritten.
   \param key
   \param value
 */
void SettingManager::setValue(const QString &key, const QVariant &value)
{
    m_Settings.setValue(key, value);
}

/*!
   \fn SettingManager::value()
   \brief Returns the value for setting key. If the setting doesn't exist, returns defaultValue.
   \param key
   \param defaultValue
 */
QVariant SettingManager::value(const QString &key, const QVariant &defaultValue) const
{
    return m_Settings.value(key, defaultValue);
}

/*!
   \fn SettingManager::remove()
   \brief Removes the setting key and any sub-settings of key.
 */
void SettingManager::remove(const QString &key)
{
    m_Settings.remove(key);
}

/*!
   \fn SettingManager::contains()
   \brief Returns true if there exists a setting called key; returns false otherwise.
 */
bool SettingManager::contains(const QString &key) const
{
    return m_Settings.contains(key);
}

/*!
   \fn SettingManager::beginGroup()
   \brief Appends prefix to the current group.
   \param prefix
   \sa endGroup
   \sa group
 */
void SettingManager::beginGroup(const QString &prefix)
{
    m_Settings.beginGroup(prefix);
}

/*!
   \fn SettingManager::endGroup()
   \brief Resets the group to what it was before the corresponding beginGroup() call.
   \sa beginGroup
   \sa group
 */
void SettingManager::endGroup()
{
    m_Settings.endGroup();
}

/*!
   \fn SettingManager::group()
   \brief Returns the current group.
   \returns The current group
   \sa beginGroup
   \sa endGroup
 */
QString SettingManager::group() const
{
    return m_Settings.group();
}


void SettingManager::registerPageFactory(ISettingPageFactory *page)
{
    m_Pages.append(page);
}

void SettingManager::settingDialog()
{
    SettingDialog dialog(m_Pages, MainWindow::MainWindow::instance());
    dialog.exec();
}



}}


