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

#include "SettingPage.h"
#include "ui_SettingPage.h"

#include <SettingManager/SettingManager.h>

#include <QDebug>
#include <QStandardItemModel>
#include <QDomDocument>
#include <QComboBox>
#include <QTableView>

namespace Plugins {
namespace OpenSpeedShop {


// Column 0 - Old path pattern
// Column 1 - New path
// Column 2 - Pattern type
// Column 3 - Case sensitivity



ComboBoxDelegate::ComboBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    QComboBox *comboBox = new QComboBox(parent);

    if(index.column() == 2) {
        comboBox->addItem(tr("Wildcard"));
        comboBox->addItem(tr("Fixed String"));
        comboBox->addItem(tr("Regular Expression"));
    } else if(index.column() == 3) {
        comboBox->addItem(tr("Case Sensitive"));
        comboBox->addItem(tr("Case Insensitive"));
    }

    return comboBox;
}
void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if(!comboBox) {
        return;
    }

    QString value = index.model()->data(index, Qt::EditRole).toString();

    for(int i = 0; i < comboBox->count(); ++i) {
        if(comboBox->itemText(i).compare(value, Qt::CaseInsensitive) == 0) {
            comboBox->setCurrentIndex(i);
            return;
        }
    }

    comboBox->setCurrentIndex(0);
}
void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if(!comboBox) {
        return;
    }

    model->setData(index, comboBox->currentText(), Qt::EditRole);
}
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}



/*!
   \class SettingPage
   \brief Widget that handles settings for the Open|SpeedShop plugin.

   This page is produced by the SettingPageFactory, which is registered
   with the SettingManager in the Core set of libraries.

   \sa OpenSpeedShopPlugin, SettingPageFactory, Core::SettingManager
 */


SettingPage::SettingPage(QWidget *parent) :
    Core::SettingManager::ISettingPage(parent),
    ui(new Ui::SettingPage)
{
    ui->setupUi(this);

    QStandardItemModel *model = new QStandardItemModel(ui->lstPathRewriting);
    ui->lstPathRewriting->setModel(model);

    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(pathRewritingItemChanged(QStandardItem*)));

    initialize();
}

SettingPage::~SettingPage()
{
    delete ui;
}

void SettingPage::initialize()
{
    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop");


    // Populate the model with data from the settings manager
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->lstPathRewriting->model());
    model->clear();

    QStringList headerLabels;
    headerLabels << tr("Path Pattern") << tr("New Path") << tr("Pattern Syntax") << tr("Case Sensitivity");
    model->setColumnCount(4);
    model->setHorizontalHeaderLabels(headerLabels);

    ComboBoxDelegate *delegate = new ComboBoxDelegate(this);
    ui->lstPathRewriting->setItemDelegateForColumn(2, delegate);
    ui->lstPathRewriting->setItemDelegateForColumn(3, delegate);

    QString pathRewritingXML = settingManager.value("Experiment/pathRewriting", QString()).toString();

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
                    QList<QStandardItem *> columns;
                    columns << new QStandardItem(element.attribute("pattern"))
                            << new QStandardItem(element.attribute("newPath"))
                            << new QStandardItem(element.attribute("patternSyntax", tr("Wildcard")))
                            << new QStandardItem(element.attribute("caseSensitivity", tr("Case Sensitive")));
                    model->appendRow(columns);

                }
                node = node.nextSibling();
            }

        } else {
            qWarning() << Q_FUNC_INFO << tr("Error reading path rewriting information from stored settings; error: \"%1\" at line: %2 column: %3").arg(error).arg(errorLine).arg(errorColumn) << pathRewritingXML;
        }
    }

    ui->lstPathRewriting->resizeColumnsToContents();


    ui->txtDatabasePath->setText(settingManager.value("Experiment/defaultExperimentPath", QString(QLatin1Char('/'))).toString());
    ui->txtSourcePath->setText(settingManager.value("Experiment/defaultSourcePath", QString(QLatin1Char('/'))).toString());

    bool okay;
    ui->txtExperimentSampleRate->setValue(settingManager.value("Experiment/defaultSampleRate", 60).toInt(&okay));
    if(!okay) { ui->txtExperimentSampleRate->setValue(60); }

    ui->txtModelRowCount->setValue(settingManager.value("ModelManager/defaultRowCount", 1000).toInt(&okay));
    if(!okay) { ui->txtModelRowCount->setValue(1000); }

    settingManager.endGroup();
}

void SettingPage::apply()
{
    // Persist changed settings to SettingManager
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop");

    // Write stuff from the model into the settings manager
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->lstPathRewriting->model());

    QDomDocument document;
    QDomElement root = document.createElement("pathRewrites");
    document.appendChild(root);

    for(int i = 0; i < model->rowCount(); ++i) {
        QDomElement pathRewrite = document.createElement("pathRewrite");
        pathRewrite.setAttribute("pattern", model->item(i, 0)->text());
        pathRewrite.setAttribute("newPath", model->item(i, 1)->text());
        pathRewrite.setAttribute("patternSyntax", model->item(i, 2)->text());
        pathRewrite.setAttribute("caseSensitivity", model->item(i, 3)->text());
        root.appendChild(pathRewrite);
    }

    QString pathRewritingXML = document.toString(0);
    settingManager.setValue("Experiment/pathRewriting", pathRewritingXML);


    settingManager.setValue("Experiment/defaultExperimentPath", ui->txtDatabasePath->text());
    settingManager.setValue("Experiment/defaultSourcePath", ui->txtSourcePath->text());
    settingManager.setValue("Experiment/defaultSampleRate", ui->txtExperimentSampleRate->value());
    settingManager.setValue("ModelManager/defaultRowCount", ui->txtModelRowCount->value());

    settingManager.endGroup();
}

void SettingPage::reset()
{
    initialize();
}


void SettingPage::on_btnPathRewritingAdd_clicked()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->lstPathRewriting->model());
    QList<QStandardItem *> columns;
    columns.append(new QStandardItem(tr("/OLD_PATH/TO/SOURCE/")));
    columns.append(new QStandardItem(tr("/NEW_PATH/TO/SOURCE/")));
    columns.append(new QStandardItem(tr("Wildcard")));
    columns.append(new QStandardItem(tr("Case Sensitive")));
    model->appendRow(columns);

    ui->lstPathRewriting->resizeColumnsToContents();
}


void SettingPage::on_btnPathRewritingRemove_clicked()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->lstPathRewriting->model());

    foreach(QModelIndex index, ui->lstPathRewriting->selectionModel()->selectedRows()) {
        model->removeRow(index.row());
    }

    ui->lstPathRewriting->resizeColumnsToContents();
}

void SettingPage::pathRewritingItemChanged(QStandardItem *item)
{
    ui->lstPathRewriting->resizeColumnToContents(item->column());
}


}}
