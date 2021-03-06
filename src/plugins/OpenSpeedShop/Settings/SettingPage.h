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

#ifndef PLUGINS_OPENSPEEDSHOP_OSSSETTINGPAGE_H
#define PLUGINS_OPENSPEEDSHOP_OSSSETTINGPAGE_H

#include <SettingManager/ISettingPage.h>

#include <QItemDelegate>
class QStandardItemModel;
class QStandardItem;

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui { class SettingPage; }

class ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class SettingPage : public Core::SettingManager::ISettingPage
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPage)

public:
    SettingPage(QWidget *parent = 0);
    ~SettingPage();

public slots:
    void apply();
    void reset();

protected:
    void initialize();

protected slots:
    void on_btnPathRewritingAdd_clicked();
    void on_btnPathRewritingRemove_clicked();
    void pathRewritingItemChanged(QStandardItem *item);

private:
    Ui::SettingPage *ui;
};


}}
#endif // PLUGINS_OPENSPEEDSHOP_OSSSETTINGPAGE_H
