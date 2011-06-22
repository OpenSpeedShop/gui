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

#include "WelcomeData.h"
#include <PluginManager/PluginManager.h>
#include "OpenSpeedShopWidget.h"

using namespace Plugins::Welcome;

namespace Plugins {
namespace OpenSpeedShop {

WelcomeData::WelcomeData(QObject *parent) :
    QObject(parent)
{
}

QList<Link> WelcomeData::actions()
{
    QList<Link> list;

    Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
    QList<OpenSpeedShopWidget *> openSpeedShopWidgets = pluginManager.getObjects<OpenSpeedShopWidget>();
    if(openSpeedShopWidgets.count() == 1) {
        OpenSpeedShopWidget *openSpeedShopWidget = openSpeedShopWidgets.at(0);

        //! \todo Enable this when we have the functionality!
//        list.append(Link(tr("Create Open|SpeedShop Experiment"),
//                         tr("create a new Open|SpeedShop experiment."),
//                         Link::Type_None, 127,
//                         openSpeedShopWidget, SLOT(createExperiment())));

        list.append(Link(tr("Load Open|SpeedShop Experiment"),
                         tr("open an existing Open|SpeedShop experiment database."),
                         Link::Type_None, 128,
                         openSpeedShopWidget, SLOT(loadExperiment())));
    }


    return list;
}

QList<Link> WelcomeData::recent()
{
    QList<Link> list;

    list.append(Link(tr("/home/dane/smg2000-pcsamp.openss"), QString(), Link::Type_None, 128,
                     QUrl("openss:///home/dane/smg2000-pcsamp.openss")));

    list.append(Link(tr("/home/dane/smg2000-io.openss"), QString(), Link::Type_None, 128,
                     QUrl("openss:///home/dane/smg2000-io.openss")));

    list.append(Link(tr("/home/dane/smg2000-iot.openss"), QString(), Link::Type_None, 128,
                     QUrl("openss:///home/dane/smg2000-iot.openss")));

    return list;
}

QList<Link> WelcomeData::tutorials()
{
    QList<Link> list;

    list.append(Link(tr("Getting Started with Open|SpeedShop"),
                     tr("a video showing the basics of the Open|SpeedShop GUI."),
                     Link::Type_Video, 16,
                     QUrl(tr("http://youtu.be/xWYRYWDpV40?hd=1"))));

    list.append(Link(tr("Performance Analysis using Open|SpeedShop"),
                     tr("a set of videos showing basic performance analysis using the Open|SpeedShop GUI."),
                     Link::Type_Video, 32,
                     QUrl(tr("http://www.youtube.com/user/openspeedshop"))));

    list.append(Link(tr("Advanced Performance Analysis using Open|SpeedShop"),
                     tr("help documentation on advanced performance analysis techniques using the Open|SpeedShop GUI."),
                     Link::Type_Text, 128,
                     QUrl(tr("qthelp://org.openspeedshop.gui/openspeedshop/advancedTechniques.html"))));

    return list;
}

QList<Link> WelcomeData::examples()
{
    QList<Link> list;

    list.append(Link(tr("SMG2000 - Semicoarsening Multigrid Solver"),
                     tr("a detailed walk-through describing how to use the Open|SpeedShop performance tools to analyze the provided test application SMG2000"),
                     Link::Type_Text, 128,
                     QUrl(tr("qthelp://org.openspeedshop.gui/openspeedshop/smg2000.html"))));

    return list;
}

QList<Link> WelcomeData::support()
{
    QList<Link> list;

    list.append(Link(tr("Get Open|SpeedShop GUI Support"),
                     tr("go to the Open|SpeedShop website for more information on getting support for the GUI."),
                     Link::Type_Web, 127,
                     QUrl(tr("http://gui.openspeedshop.org/support"))));

    list.append(Link(tr("Get Open|SpeedShop Support"),
                     tr("go to the Open|SpeedShop website for more information on getting general support."),
                     Link::Type_Web, 128,
                     QUrl(tr("http://openspeedshop.org/support"))));

    return list;
}

QStringList WelcomeData::tipsAndTricks()
{
    QStringList list;

    //! \todo Pull this list from an xml file!
    list << tr("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
    list << tr("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    list << tr("Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
    list << tr("Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

    return list;
}

QList<QUrl> WelcomeData::latestNewsRss()
{
    QList<QUrl> list;

    list.append(QUrl("http://www.openspeedshop.org/wp/feed/"));

    return list;
}

QList<QWidget *> WelcomeData::additionalTabs()
{
    QList<QWidget *> list;
    return list;
}


} // namespace OpenSpeedShop
} // namespace Plugins
