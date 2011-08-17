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

#include "WelcomeWidget.h"
#include "ui_WelcomeWidget.h"

#include <QMetaClassInfo>
#include <QDesktopServices>
#include <QDateTime>
#include <QScrollArea>
#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>

#include <QDebug>

namespace Plugins {
namespace Welcome {

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    setObjectName("welcomeWidget");

    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/Welcome");
    setNewsTabVisible(settingManager.value("rssEnabled", true).toBool());
    settingManager.endGroup();

    QScrollArea *scrollArea = new QScrollArea(ui->tabNews);
    scrollArea->setWidgetResizable(true);
    m_RssReaderWidget = new RssReaderWidget(scrollArea);
    scrollArea->setWidget(m_RssReaderWidget);
    ui->layNews->addWidget(scrollArea);

    connect(&m_UrlMapper, SIGNAL(mapped(int)), this, SLOT(urlClicked(int)));

    /* Check the object pool for anything we should manage */
    Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
    foreach(QObject *object, pluginManager.allObjects()) { pluginObjectRegistered(object); }
    connect(&pluginManager, SIGNAL(objectAdded(QObject*)), this, SLOT(pluginObjectRegistered(QObject*)));
    connect(&pluginManager, SIGNAL(objectRemoving(QObject*)), this, SLOT(pluginObjectDeregistered(QObject*)));
}

WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}


void WelcomeWidget::pluginObjectRegistered(QObject *object)
{
    IWelcomeData *welcomeData = qobject_cast<IWelcomeData *>(object);
    if(welcomeData) registerWelcomeData(welcomeData);
}

void WelcomeWidget::pluginObjectDeregistered(QObject *object)
{
    IWelcomeData *welcomeData = qobject_cast<IWelcomeData *>(object);
    if(welcomeData) deregisterWelcomeData(welcomeData);
}

void WelcomeWidget::registerWelcomeData(IWelcomeData *welcomeData)
{
    if(!m_WelcomeData.contains(welcomeData)) {
        m_WelcomeData.append(welcomeData);

        foreach(Link link, welcomeData->actions()) {
            addCommandButton(link, ui->grpActions);
        }

        foreach(Link link, welcomeData->tutorials()) {
            addCommandButton(link, ui->grpTutorials);
        }

        foreach(Link link, welcomeData->examples()) {
            addCommandButton(link, ui->grpExamples);
        }

        foreach(Link link, welcomeData->support()) {
            addCommandButton(link, ui->grpSupport);
        }

        m_TipsAndTricks.append(welcomeData->tipsAndTricks());
        randomTip();

        if(ui->tabNews->isEnabled()) {
            foreach(QUrl rss, welcomeData->latestNewsRss()) {
                m_RssReaderWidget->addFeed(rss);
            }
        }

        foreach(Link link, welcomeData->recent()) {
            QListWidgetItem *item = new QListWidgetItem(link.title, ui->lstRecent);
            item->setData(Qt::UserRole+1, link.priority);

            item->setToolTip(link.description);
            if(link.url.isValid()) {
                //TODO: Connect where we will handle the url
            } else if(link.receiver && link.method) {
                //TODO: Handle the click event
            }

            //TODO: sort by priority (which should be the time it was last accessed)
            ui->lstRecent->addItem(item);
        }

        foreach(QWidget *tab, welcomeData->additionalTabs()) {
            ui->tabWidget->addTab(tab, tab->windowIcon(), tab->windowTitle());
        }
    }
}

void WelcomeWidget::addCommandButton(const Link &link, QWidget *parent)
{
    QCommandLinkButton *btn = new QCommandLinkButton(link.title, link.description, parent);

    if(link.url.isValid()) {
        connect(btn, SIGNAL(clicked()), &m_UrlMapper, SLOT(map()));
        m_UrlMapper.setMapping(btn, m_Urls.count());
        m_Urls.append(link.url);

        /* There's no way to see what is registered with the QDesktopServices::setUrlHandler(),
           otherwise we should use that instead of manually hardcoding. */
        if(link.url.scheme() != "qthelp") {
            btn->setToolTip(link.url.toString());
        }

    } else if(link.receiver && link.method) {
        connect(btn, SIGNAL(clicked()), link.receiver, link.method);
    }

    switch(link.type) {
    case Link::Type_Text:
        btn->setIcon(QIcon(":/Welcome/text.svg"));
        break;
    case Link::Type_Video:
        btn->setIcon(QIcon(":/Welcome/video.svg"));
        break;
    case Link::Type_Web:
        btn->setIcon(QIcon(":/Welcome/link.svg"));
        break;
    default:
         /* Gets rid of stupid compiler warnings; it's like GNU expects coders to not understand switch statements. */
        break;
    }

    /* Insert the item into the parent based on priority */
    bool insertedWidget = false;
    btn->setProperty("priority", link.priority);
    QBoxLayout *layout = qobject_cast<QBoxLayout *>(parent->layout());
    for(int index=0; index < layout->count(); ++index) {
        QLayoutItem *layoutItem = layout->itemAt(index);
        QWidget *widgetItem = layoutItem->widget();
        bool okay;
        if(widgetItem) {
            int itemPriority = widgetItem->property("priority").toInt(&okay);
            if(okay && itemPriority > link.priority) {
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

void WelcomeWidget::deregisterWelcomeData(IWelcomeData *welcomeData)
{
    if(m_WelcomeData.contains(welcomeData)) {
        m_WelcomeData.removeOne(welcomeData);

        /* Remove the data from the widget */
    }
}

void WelcomeWidget::urlClicked(int index)
{
    QDesktopServices::openUrl(m_Urls.at(index));
}

void WelcomeWidget::setCurrentTip(int index) {
    if(m_TipsAndTricks.count() < 1)
        return;

    if(index >= m_TipsAndTricks.count()) {
        m_CurrentTip = 0;
    } else if(index < 0) {
        m_CurrentTip = m_TipsAndTricks.count()-1;
    } else {
        m_CurrentTip = index;
    }

    ui->lblTip->setText(m_TipsAndTricks.at(m_CurrentTip));
}

void WelcomeWidget::randomTip()
{
    qsrand((uint)QDateTime::currentDateTime().time().second());
    setCurrentTip(qrand() % m_TipsAndTricks.count());
}

void WelcomeWidget::on_btnTipNext_clicked()
{
    setCurrentTip(m_CurrentTip+1);
}

void WelcomeWidget::on_btnTipPrevious_clicked()
{
    setCurrentTip(m_CurrentTip-1);
}

QStringList WelcomeWidget::rssNewsFeeds()
{
    return m_RssReaderWidget->rssFeeds();
}

void WelcomeWidget::setNewsTabVisible(bool visible)
{
    static int index = 0;

    if(visible && !ui->tabNews->isEnabled()) {
        ui->tabNews->setEnabled(true);
        ui->tabWidget->insertTab(index, ui->tabNews, "Latest News");
    } else if(!visible && ui->tabNews->isEnabled()) {
        ui->tabNews->setEnabled(false);
        index = ui->tabWidget->indexOf(ui->tabNews);
        ui->tabWidget->removeTab(index);
    }
}

} // namespace Welcome
} // namespace Plugins
