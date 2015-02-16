/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Open|SpeedShop Graphical User Interface
   Copyright (C) 2010-2015 Argo Navis Technologies, LLC

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

#ifndef PLUGINS_OPENSPEEDSHOP_PATHREWRITER_H
#define PLUGINS_OPENSPEEDSHOP_PATHREWRITER_H

#include <QObject>
#include <QMap>
#include <QRegExp>

class RegExpReplacement : public QRegExp
{
public:
    QString replacement() const;
    void setReplacement(const QString &replacement);
private:
    QString m_Replacement;
};

class PathRewriter : public QObject
{
    Q_OBJECT
public:
    static PathRewriter &instance();

    bool initialize(QStringList &args, QString *err);
    void shutdown();

    QString rewrite(const QString &oldPath);
    void setRewrite(const QString &oldPath, const QString &newPath);
    int hasRewrite(const QString &oldPath);

protected:
    explicit PathRewriter(QObject *parent = 0);
    ~PathRewriter();

    void restorePathRewrite();

    void restorePathCache();
    void storePathCache();

private:
    QMap<QString, QString> m_PathCache;
    QList<RegExpReplacement> m_PathRewrite;

};

#endif // PLUGINS_OPENSPEEDSHOP_PATHREWRITER_H
