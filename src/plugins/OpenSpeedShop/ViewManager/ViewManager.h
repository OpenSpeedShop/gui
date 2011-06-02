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

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>
#include "ViewManagerLibrary.h"
#include "IViewPlugin.h"

namespace Plugins {
namespace OpenSpeedShop {

class VIEWMANAGER_EXPORT ViewManager : public QObject
{
    Q_OBJECT
public:
    static ViewManager *instance();

    void initialize();
    void shutdown();

    void registerView(IViewPlugin *viewPlugin);
    QStringList viewNames(QAbstractItemModel *model = NULL);
    QAbstractItemView *viewWidget(QString name, QAbstractItemModel *model);

protected:
    ViewManager(QObject *parent = 0);
    QMap<QString, IViewPlugin *> m_viewPlugins;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // VIEWMANAGER_H
