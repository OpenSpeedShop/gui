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

#include "PathRewriter.h"

#include <MainWindow/MainWindow.h>
#include <PluginManager/PluginManager.h>

#ifdef QT_DEBUG
# include <QtDebug>
#endif

PathRewriter &PathRewriter::instance()
{
    static PathRewriter m_Instance;
    return m_Instance;
}

PathRewriter::PathRewriter(QObject *parent) :
    QObject(parent)
{
}

PathRewriter::~PathRewriter()
{
}

bool PathRewriter::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {

        restorePathCache();

        using namespace Core::PluginManager;
        PluginManager &pluginManager = PluginManager::instance();
        pluginManager.addObject(this);

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Could not initialize remote file path rewriter: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Could not initialize remote file path rewriter."), NotificationWidget::Critical);
    }

    return true;
}

void PathRewriter::shutdown()
{
    try {

        storePathCache();

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Could not shut down remote file path rewriter: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Could not shut down remote file path rewriter."), NotificationWidget::Critical);
    }
}

void PathRewriter::restorePathCache()
{
    QString filePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    filePath.append("/PathCache.xml");

    QFile file(filePath);
    if(!file.exists()) {
#ifdef QT_DEBUG
        throw tr("Path cache file does not exist: %1").arg(filePath);
#else
        return;
#endif
    }

    QDomDocument document = QDomDocument("PathCache");
    if (!file.open(QIODevice::ReadOnly)) {
        throw tr("Could not open path cache file: %1").arg(filePath);
    }
    if (!document.setContent(&file)) {
        file.close();
        throw tr("Could not use path cache file after opening --possibly invalid text");
    }
    file.close();

    QDomElement rootElement = document.firstChildElement("PathRewrites");

    QDomElement pathElement = rootElement.firstChildElement("Path");
    while(!pathElement.isNull()) {
        QString oldPath = pathElement.attribute("oldPath");
        QString newPath = pathElement.attribute("newPath");

        if(!oldPath.isEmpty() && !newPath.isEmpty()) {
            m_PathCache[oldPath] = newPath;
        }

        pathElement = pathElement.nextSiblingElement(pathElement.tagName());
    }
}

void PathRewriter::storePathCache()
{
    QDir dataLocation(QDesktopServices::storageLocation(QDesktopServices::DataLocation));

    if(!dataLocation.exists()) {
        if(!dataLocation.mkpath(dataLocation.path())) {
            throw tr("Could not create data location path to store path cache: %1").arg(dataLocation.path());
        }
    }

    QDomDocument document = QDomDocument("PathCache");
    QDomElement rootElement = document.createElement("PathRewrites");
    document.appendChild(rootElement);

    foreach(QString oldPath, m_PathCache.keys()) {
        QString newPath = m_PathCache.value(oldPath, QString());

        if(!oldPath.isEmpty() && !newPath.isEmpty()) {
            QDomElement pathElement = document.createElement("Path");
            pathElement.setAttribute("oldPath", oldPath);
            pathElement.setAttribute("newPath", newPath);
            rootElement.appendChild(pathElement);
        }
    }

    QFile file(dataLocation.absoluteFilePath("PathCache.xml"));
    if (!file.open(QIODevice::WriteOnly)) {
        throw tr("Could not open path cache file: %1").arg(file.fileName());
    }

    QByteArray buffer = document.toString(-1).toAscii();
    qint64 written = file.write(buffer);
    if(written != buffer.count()) {
        file.close();
        throw tr("Could not write to path cache file after opening");
    }

    file.close();
}

QString PathRewriter::rewrite(const QString &oldPath)
{
    QString newPath;
    if(m_PathCache.contains(oldPath)) {
        newPath = m_PathCache.value(oldPath, QString());
    }

    if(newPath.isEmpty()) {
        newPath = oldPath;
    }

    return newPath;
}

void PathRewriter::setRewrite(const QString &oldPath, const QString &newPath)
{
    if(m_PathCache.contains(oldPath)) {
        m_PathCache.remove(oldPath);
    }

    if(!newPath.isEmpty()) {
        m_PathCache.insert(oldPath, newPath);
    }
}

bool PathRewriter::hasRewrite(const QString &oldPath)
{
    return m_PathCache.contains(oldPath);
}
