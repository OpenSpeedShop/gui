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

#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <PluginManager/PluginManager.h>
#include "OpenSpeedShopWidget.h"

using namespace Plugins::Welcome;

namespace Plugins {
namespace OpenSpeedShop {

WelcomeData::WelcomeData(QObject *parent) :
    QObject(parent),
    m_WelcomeData("WelcomeData")
{
}

bool WelcomeData::initialize()
{
#ifdef WIN32
    QFileInfo fileInfo(QString("%1/oss/WelcomeData.xml").arg(QApplication::instance()->applicationDirPath()));
#else
    QFileInfo fileInfo(QString("%1/../share/openspeedshop/gui/WelcomeData.xml").arg(QApplication::instance()->applicationDirPath()));
#endif

    QString filePath;
//    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
//    settingManager.beginGroup("Plugins/OpenSpeedShop/Welcome");
//    filePath = settingManager.value("welcomeDataPath", fileInfo.absoluteFilePath()).toString();
//    settingManager.endGroup();

    if(filePath.isEmpty()) {
        filePath = fileInfo.absoluteFilePath();
    }

//    bool defaultPath = (filePath == fileInfo.absoluteFilePath());

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString error = tr("Could not open welcome data file: %1").arg(filePath);
        qWarning(error.toLatin1());
        throw error;
    }
    if (!m_WelcomeData.setContent(&file)) {
        file.close();
        QString error = tr("Could not use welcome data file after opening, possibly invalid text: %1").arg(filePath);
        qWarning(error.toLatin1());
        throw error;
    }
    file.close();

//    /* User convenience.  If the path wasn't set in the settings, we'll set it for the user the first time. */
//    if(defaultPath) {
//        settingManager.beginGroup("Plugins/OpenSpeedShop/Welcome");
//        settingManager.setValue("welcomeDataPath", filePath);
//        settingManager.endGroup();
//    }

    return true;
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
    return createUrlLinks("Tutorials");
}

QList<Link> WelcomeData::examples()
{
    return createUrlLinks("Examples");
}

QList<Link> WelcomeData::support()
{
    return createUrlLinks("Support");
}

QStringList WelcomeData::tipsAndTricks()
{
    QStringList list;

    QDomElement welcomeData = m_WelcomeData.firstChildElement("WelcomeData");
    if(welcomeData.isNull()) {
        return list;
    }

    QDomElement element = welcomeData.firstChildElement("TipsAndTricks");
    if(element.isNull()) {
        return list;
    }

    QDomElement tip = element.firstChildElement("Tip");
    while(!tip.isNull()) {
        list.append(tip.attribute("text"));
        tip = tip.nextSiblingElement(tip.tagName());
    }

    return list;
}

QList<QUrl> WelcomeData::latestNewsRss()
{
    QList<QUrl> list;

    QDomElement welcomeData = m_WelcomeData.firstChildElement("WelcomeData");
    if(welcomeData.isNull()) {
        return list;
    }

    QDomElement element = welcomeData.firstChildElement("LatestNews");
    if(element.isNull()) {
        return list;
    }

    QDomElement rss = element.firstChildElement("Rss");
    while(!rss.isNull()) {
        list.append(QUrl(rss.attribute("url")));
        rss = rss.nextSiblingElement(rss.tagName());
    }

    return list;
}

QList<QWidget *> WelcomeData::additionalTabs()
{
    QList<QWidget *> list;
    return list;
}

QList<Link> WelcomeData::createUrlLinks(const QString &elementName)
{
    QList<Link> list;

    QDomElement welcomeData = m_WelcomeData.firstChildElement("WelcomeData");
    if(welcomeData.isNull()) {
        return list;
    }

    QDomElement element = welcomeData.firstChildElement(elementName);
    if(element.isNull()) {
        return list;
    }

    QDomElement link = element.firstChildElement("Link");
    while(!link.isNull()) {
        list.append(createUrlLink(link));
        link = link.nextSiblingElement(link.tagName());
    }

    return list;
}

Link WelcomeData::createUrlLink(const QDomElement &link)
{
    QString title = link.attribute("title");
    QString description = link.attribute("description");

    QString typeString = link.attribute("type");
    Link::Type type = Link::Type_None;
    if(typeString == "text") {
        type = Link::Type_Text;
    } else if(typeString == "video") {
        type = Link::Type_Video;
    } else if(typeString == "web") {
        type = Link::Type_Web;
    }

    bool okay;
    int priority = link.attribute("priority", "128").toInt(&okay);
    if(!okay) priority = 128;

    QUrl url(link.attribute("url"));

    return Link(title, description, type, priority, url);
}


} // namespace OpenSpeedShop
} // namespace Plugins
