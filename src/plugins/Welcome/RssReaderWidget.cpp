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

#include "RssReaderWidget.h"
#include <QXmlStreamReader>
#include <QCommandLinkButton>
#include <QDateTime>
#include <QLayoutItem>
#include <QBoxLayout>
#include <QDesktopServices>
#include <QTextDocument>

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace Welcome {

RssReaderWidget::RssReaderWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    setLayout(layout);

    connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    connect(&m_NetworkManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(&m_UrlMapper, SIGNAL(mapped(int)), this, SLOT(urlClicked(int)));
}

void RssReaderWidget::addFeed(QUrl url)
{
    m_Feeds.append(url);
    getFeed(url);
}

QStringList RssReaderWidget::rssFeeds()
{
    QStringList list;
    foreach(QUrl url, m_Feeds) {
        list.append(url.toString());
    }
    return list;
}

void RssReaderWidget::getFeed(QUrl url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_NetworkManager.get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    m_Replies.append(reply);
}

void RssReaderWidget::metaDataChanged()
{
    QObject *object = QObject::sender();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(object);
    if(reply) {
        /* Redirect to the new one */
        QUrl redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (redirectionTarget.isValid()) {
            getFeed(redirectionTarget);
        }
    }
}

void RssReaderWidget::readyRead()
{
    QObject *object = QObject::sender();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(object);
    if(reply) {

        //TODO: This should be done properly, using a QDomDocument
        QXmlStreamReader feedXml;

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode >= 200 && statusCode < 300) {
            feedXml.addData(reply->readAll());

            QString currentTag;
            QString link;
            QString title;
            QString description;
            QString date;

            while(!feedXml.atEnd()) {
                feedXml.readNext();

                if(feedXml.isStartElement()) {
                    if(feedXml.name() == "item") {
                        title.clear();
                        link.clear();
                        description.clear();
                        date.clear();
                    }
                    currentTag = feedXml.name().toString();

                } else if(feedXml.isEndElement()) {
                    if(feedXml.name() == "item") {
                        QCommandLinkButton *btn = new QCommandLinkButton(this);

                        /* Clean up the titles a little bit */
                        title.replace('\t', "  ");
                        title.replace('\n', "  ");
                        title = title.trimmed();

                        /* Clean up the descriptions a little bit */
                        description.replace('\t', "  ");
                        description.replace('\n', "  ");
                        description = description.trimmed();

                        /* Clean up the date a little bit */
                        date.replace('\t', "  ");
                        date.replace('\n', "  ");
                        date = date.trimmed();

                        btn->setText(title);
                        btn->setIcon(QIcon());

                        /* Get the date time and priority of this item */
                        quint64 priority = 0;
                        // Try for ISO8601
                        QDateTime dateTime = QDateTime::fromString(date, Qt::ISODate);
                        // Try for RFC2822 (email dateTime)
                        if(!dateTime.isValid()) {
                            static QRegExp pubDatePattern("^(?:\\S+,\\s)(.*)\\s(\\S+)$");
                            if(pubDatePattern.exactMatch(date)) {
                                QString pubDateString = pubDatePattern.cap(1);
                                int pubDateOffset = pubDatePattern.cap(2).toInt();
                                dateTime = QDateTime::fromString(pubDateString, "dd MMM yyyy HH:mm:ss");
                                dateTime.addSecs(pubDateOffset * 3600);
                                dateTime.setTimeSpec(Qt::UTC);
                            }
                        }

                        if(dateTime.isValid()) {
                            priority = dateTime.toTime_t();
                            btn->setDescription(QString("[%1] %2").arg(dateTime.date().toString(Qt::SystemLocaleShortDate), description));
                        } else {
                            btn->setDescription(QString("%2").arg(description));
                        }

                        /* Get a valid link and wire it up so that when the user clicks the button, they are taken to it */
                        QUrl url(link);
                        if(url.isValid()) {
                            connect(btn, SIGNAL(clicked()), &m_UrlMapper, SLOT(map()));
                            m_UrlMapper.setMapping(btn, m_Urls.count());
                            m_Urls.append(url);
                            btn->setToolTip(url.toString());
                        }

                        /* Insert the item into the parent based on priority */
                        btn->setProperty("priority", priority);
                        bool insertedWidget = false;
                        QBoxLayout *layout = qobject_cast<QBoxLayout *>(this->layout());
                        for(int index = 0; index < layout->count(); ++index) {
                            QLayoutItem *layoutItem = layout->itemAt(index);
                            QWidget *widgetItem = layoutItem->widget();
                            bool okay;
                            if(widgetItem) {
                                quint64 itemPriority = widgetItem->property("priority").toULongLong(&okay);
                                if(okay && itemPriority < priority) {
                                    layout->insertWidget(index, btn);
                                    insertedWidget = true;
                                    break;
                                }
                            }
                        }
                        if(!insertedWidget) {
                            layout->addWidget(btn);
                        }
                    }

                } else if(feedXml.isCharacters() && !feedXml.isWhitespace()) {
                    if(currentTag == "title") {
                        title += feedXml.text().toString();
                    } else if(currentTag == "link") {
                        link += feedXml.text().toString();
                    } else if(currentTag == "description") {
                        /* Convert from HTML to plain text */
                        QTextDocument doc;
                        doc.setHtml(feedXml.text().toString());
                        description += doc.toPlainText();
                    } else if(currentTag == "pubDate") {
                        date += feedXml.text().toString();
                    } else if(currentTag == "date") {
                        date += feedXml.text().toString();
                    }
                }
            }

#ifdef QT_DEBUG
            if (feedXml.error() && feedXml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
                qWarning() << "XML ERROR:" << feedXml.lineNumber() << ": " << feedXml.errorString();
            }
#endif

        }


    }

}

void RssReaderWidget::error(QNetworkReply::NetworkError)
{
#ifdef QT_DEBUG
    QObject *object = QObject::sender();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(object);
    if(reply) {
        qWarning() << "error retrieving RSS feed:" << reply->url().toString();
    }
#endif
}

void RssReaderWidget::finished(QNetworkReply *reply)
{
    /* Get rid of the reply */
    if(m_Replies.contains(reply)) {
        m_Replies.removeOne(reply);
    }
    reply->disconnect(this);
    reply->deleteLater();
}

void RssReaderWidget::sslErrors(QNetworkReply *reply, QList<QSslError> errors)
{
    //TODO: Ask the user if they will accept the certificates
    foreach(QSslError error, errors) {
        qDebug() << "SSL Error:" << error.errorString();
    }

    reply->ignoreSslErrors();
}

void RssReaderWidget::urlClicked(int index)
{
    QDesktopServices::openUrl(m_Urls.at(index));
}


} // namespace Welcome
} // namespace Plugins
