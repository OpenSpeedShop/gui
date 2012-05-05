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

#ifndef PATHREWRITER_H
#define PATHREWRITER_H

#include <QtCore>
#include <QtXml>

class PathRewriter : public QObject
{
    Q_OBJECT
public:
    static PathRewriter &instance();

    bool initialize(QStringList &args, QString *err);
    void shutdown();

    QString rewrite(const QString &oldPath);
    void setRewrite(const QString &oldPath, const QString &newPath);
    bool hasRewrite(const QString &oldPath);

protected:
    explicit PathRewriter(QObject *parent = 0);
    ~PathRewriter();

    void restorePathCache();
    void storePathCache();

private:
    QMap<QString, QString> m_PathCache;

};

#endif // PATHREWRITER_H
