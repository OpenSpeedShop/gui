/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2013 Argo Navis Technologies, LLC

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

#ifndef PLUGINS_OPENSPEEDSHOP_WELCOMEDATA_H
#define PLUGINS_OPENSPEEDSHOP_WELCOMEDATA_H

#include <QDomElement>
#include <QDomDocument>

#include <Welcome/IWelcomeData.h>

using namespace Plugins::Welcome;

namespace Plugins {
namespace OpenSpeedShop {

class WelcomeData : public QObject, public IWelcomeData
{
    Q_OBJECT
    Q_INTERFACES(Plugins::Welcome::IWelcomeData)

public:
    WelcomeData(QObject *parent = 0);

    /* BEGIN IWelcomeData */
    bool initialize();
    QList<Link> actions();
    QList<Link> recent();
    QList<Link> tutorials();
    QList<Link> examples();
    QList<Link> support();
    QStringList tipsAndTricks();
    QList<QUrl> latestNewsRss();
    QList<QWidget *> additionalTabs();
    /* END IWelcomeData */

protected:
    QList<Link> createUrlLinks(const QString &elementName);
    Link createUrlLink(const QDomElement &link);

private:
    QDomDocument m_WelcomeData;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // PLUGINS_OPENSPEEDSHOP_WELCOMEDATA_H
