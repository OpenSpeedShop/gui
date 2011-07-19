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

#ifndef RSSREADERWIDGET_H
#define RSSREADERWIDGET_H

#include <QWidget>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSignalMapper>
#include <QSslError>
#include <QSslSocket>

namespace Plugins {
namespace Welcome {


class RssReaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RssReaderWidget(QWidget *parent = 0);
    void addFeed(QUrl url);

    QStringList rssFeeds();

protected slots:
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply*,QList<QSslError>);
    void getFeed(QUrl url);
    void metaDataChanged();
    void readyRead();
    void error(QNetworkReply::NetworkError);
    void urlClicked(int index);

private:
    QNetworkAccessManager m_NetworkManager;
    QList<QUrl> m_Feeds;
    QList<QUrl> m_Urls;
    QList<QNetworkReply *> m_Replies;

    QSignalMapper m_UrlMapper;


};

} // namespace Welcome
} // namespace Plugins

#endif // RSSREADERWIDGET_H
