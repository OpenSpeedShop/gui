/*!
   \file PluginManager.h
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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>

#include <SettingManager/SettingManager.h>

#include "IPlugin.h"
#include "PluginWrapper.h"

namespace Core {
namespace PluginManager {

class PluginManager : public QObject
{
    Q_OBJECT
public:
    static PluginManager *instance();

    bool initialize();
    bool initialized();

    void loadPlugins();

    void addObject(QObject *object);
    bool delObject(QObject *object);

    template <typename Type> QList<Type *> *getObjects() const {
        QList<Type *> *list = QList<Type *>();
        Type *type = 0;
        foreach (QObject *object, m_Objects) {
            if (type = qobject_cast<Type>(object))
                list->append(type);
        }
        return list;
    }

signals:
    void pluginLoaded(IPlugin *);
    void pluginInitialized(IPlugin *);
    void pluginShutdown(IPlugin *);
    void objectAdding(QObject *);
    void objectAdded(QObject *);
    void objectRemoving(QObject *);
    void objectRemoved(QObject *);

public slots:
    void pluginDialog();

protected:
    PluginManager();
    ~PluginManager();

    bool m_Initialized;

    void readSettings();
    void writeSettings();

    void initializePlugins();

    QList<PluginWrapper *> m_Plugins;
    QList<QObject *> m_Objects;

    QString m_PluginPath;

    PluginWrapper *findPlugin(QString name);
};

}}
#endif // PLUGINMANAGER_H
