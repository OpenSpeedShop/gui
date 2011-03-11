/*!
   \file ExamplePlugin.h
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

#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include <QtCore>
#include <PluginManager/IPlugin.h>

namespace Plugins {
namespace Example {

class ExamplePlugin : public QObject, public IPlugin {
Q_OBJECT
Q_INTERFACES(IPlugin)

public:
    ExamplePlugin();

	/* IPlugin Interface */
    ~ExamplePlugin();
    bool initialize(QStringList &args, QString *err);
    void shutdown();
    QString name();
    QString version();
    QList<Dependency> dependencies();

protected:
    QString m_Name;
    QStringList m_Dependencies;

};



}}
#endif // EXAMPLEPLUGIN_H