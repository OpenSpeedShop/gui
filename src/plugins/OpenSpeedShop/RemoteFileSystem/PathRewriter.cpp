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

#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>

#if QT_VERSION >= 0x050000
#  include <QStandardPaths>
#else
#  include <QDesktopServices>
#endif

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QIcon>
#include <QDebug>


QString RegExpReplacement::replacement() const
{
    return m_Replacement;
}
void RegExpReplacement::setReplacement(const QString &replacement)
{
    m_Replacement = replacement;
}


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
        restorePathRewrite();

        using namespace Core::PluginManager;
        PluginManager &pluginManager = PluginManager::instance();
        pluginManager.addObject(this);

    } catch(QString err) {
        qCritical() << tr("Could not initialize remote file path rewriter: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Could not initialize remote file path rewriter.");
    }

    return true;
}

void PathRewriter::shutdown()
{
    try {

        storePathCache();

    } catch(QString err) {
        qCritical() << tr("Could not shut down remote file path rewriter: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Could not shut down remote file path rewriter.");
    }
}

void PathRewriter::restorePathRewrite()
{
    m_PathRewrite.clear();

    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop");

    QString pathRewritingXML = settingManager.value("Experiment/pathRewriting", QString()).toString();

    qDebug() << settingManager.group() << pathRewritingXML;

    if(!pathRewritingXML.isEmpty()) {
        QDomDocument document;
        QString error;
        int errorLine, errorColumn;
        if(document.setContent(pathRewritingXML, &error, &errorLine, &errorColumn)) {
            QDomNode root = document.firstChild();

            QDomNode node = root.firstChild();
            while(!node.isNull()) {
                QDomElement element = node.toElement();
                if(!element.isNull() && element.tagName().compare("pathRewrite") == 0) {


                    RegExpReplacement replacement;
                    replacement.setPattern(element.attribute("pattern"));
                    replacement.setReplacement(element.attribute("newPath"));

                    QString patternSyntax = element.attribute("patternSyntax", tr("Wildcard"));
                    if(patternSyntax.compare(tr("Fixed String"), Qt::CaseInsensitive) == 0) {
                        replacement.setPatternSyntax(QRegExp::FixedString);
                        qDebug() << patternSyntax << "Fixed";
                    } else if(patternSyntax.compare(tr("Regular Expression"), Qt::CaseInsensitive) == 0) {
                        replacement.setPatternSyntax(QRegExp::RegExp2);
                        qDebug() << patternSyntax << "Regexp";
                    } else {
                        replacement.setPatternSyntax(QRegExp::Wildcard);
                        qDebug() << patternSyntax << "wild";
                    }

                    QString caseSensitivity = element.attribute("caseSensitivity");
                    if(caseSensitivity.compare(tr("Case Insensitive"), Qt::CaseInsensitive) == 0) {
                        replacement.setCaseSensitivity(Qt::CaseInsensitive);
                        qDebug() << caseSensitivity << "Insense";
                    } else {
                        replacement.setCaseSensitivity(Qt::CaseSensitive);
                        qDebug() << caseSensitivity << "Sense";
                    }

                    m_PathRewrite.append(replacement);
                }
                node = node.nextSibling();
            }

        } else {
            qWarning() << Q_FUNC_INFO << tr("Error reading path rewriting information from stored settings; error: \"%1\" at line: %2 column: %3").arg(error).arg(errorLine).arg(errorColumn) << pathRewritingXML;
        }
    }


}


void PathRewriter::restorePathCache()
{
#if QT_VERSION >= 0x050000
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString filePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

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
#if QT_VERSION >= 0x050000
    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#else
    QDir dataLocation(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif


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

    QByteArray buffer = document.toString(-1).toLocal8Bit();
    qint64 written = file.write(buffer);
    if(written != buffer.count()) {
        file.close();
        throw tr("Could not write to path cache file after opening");
    }

    file.close();
}

QString PathRewriter::rewrite(const QString &oldPath)
{
    if(m_PathCache.contains(oldPath)) {
        return m_PathCache.value(oldPath, QString());
    }

    QString newPath = oldPath;
    foreach(RegExpReplacement rx, m_PathRewrite) {
        newPath = newPath.replace(rx, rx.replacement());
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

int PathRewriter::hasRewrite(const QString &oldPath)
{
    if(m_PathCache.contains(oldPath)) {
        return 1;
    }

    foreach(RegExpReplacement rx, m_PathRewrite) {
        if(oldPath.indexOf(rx) >= 0) {
            return 2;
        }
    }

    return 0;
}
