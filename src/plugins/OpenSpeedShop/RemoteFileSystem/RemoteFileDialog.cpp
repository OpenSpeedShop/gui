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

#include "RemoteFileDialog.h"
#include "ui_RemoteFileDialog.h"

#include <ConnectionManager/IAdapter.h>
#include <ConnectionManager/ConnectionManager.h>

namespace Plugins {
namespace OpenSpeedShop {

RemoteFileDialog::RemoteFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteFileDialog),
    m_Filter()
{
    ui->setupUi(this);
    setPath("/");

    m_Filter.setCaseSensitivity(Qt::CaseSensitive);
    m_Filter.setPatternSyntax(QRegExp::WildcardUnix);

    ui->buttonBox->setFocus();
}

RemoteFileDialog::~RemoteFileDialog()
{
    delete ui;
}

QString RemoteFileDialog::path() const
{
    return ui->txtPath->text();
}

void RemoteFileDialog::setPath(const QString &path)
{
    if(!ui->txtPath->text().compare(path)) {
        return;
    }

    ui->txtPath->setText(path);
    on_txtPath_editingFinished();
}

QString RemoteFileDialog::filter() const
{
    return m_Filter.pattern();
}

void RemoteFileDialog::setFilter(const QString &filter)
{
    m_Filter.setPattern(filter);
    on_txtPath_editingFinished();
}


QString RemoteFileDialog::selectedFilePath() const
{
    return m_SelectedFilePath;
}

void RemoteFileDialog::on_txtPath_editingFinished()
{
    try {

        ui->treeWidget->clear();

        QString path = this->path();
        if(!path.endsWith(QLatin1Char('/'))) {
            path.append(QLatin1Char('/'));
        }

        /* Connect to the server, and get the listing */
        IAdapter *adapter = ConnectionManager::instance().askAdapter();
        if(!adapter) throw tr("Server not connected");
        QStringList listing = adapter->waitDirStat(path);

        foreach(QString fileName, listing) {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(ui->treeWidget);
            fileItem->setText(0, fileName);

            if(fileName.endsWith(QLatin1Char('/'))) {
                fileItem->setIcon(0, QIcon(":/OpenSpeedShop/folder.svg"));
                fileItem->setData(0, Qt::UserRole, "Directory");

            } else {
                fileItem->setIcon(0, QIcon(":/OpenSpeedShop/file.svg"));
                fileItem->setData(0, Qt::UserRole, "File");

                if(!fileName.contains(m_Filter)) {
                    fileItem->setDisabled(true);
                }
            }

            fileItem->setData(0, Qt::UserRole+1, path);
            fileItem->setData(0, Qt::UserRole+2, path + fileName);

            ui->treeWidget->addTopLevelItem(fileItem);
        }

        ui->treeWidget->sortItems(0, Qt::AscendingOrder);

    } catch(QString err) {
        //TODO: Handle exception    tr("Failed open path: %1").arg(err)
    } catch(...) {
        //TODO: Handle exception    tr("Failed open path.")
    }

}

void RemoteFileDialog::on_treeWidget_activated(QModelIndex index)
{
    if(!index.isValid()) {
        m_SelectedFilePath.clear();
        return;
    }

    QString type(index.data(Qt::UserRole).toString());
    m_SelectedFilePath = index.data(Qt::UserRole+2).toString();

    if(!type.compare("directory", Qt::CaseInsensitive)) {
        setPath(m_SelectedFilePath);
    } else {
        accept();
    }
}

void RemoteFileDialog::on_treeWidget_itemSelectionChanged()
{
    QList<QTreeWidgetItem *> selectedItems = ui->treeWidget->selectedItems();
    if(selectedItems.count() == 1) {
        ui->txtFileName->setText(selectedItems.at(0)->data(0, Qt::DisplayRole).toString());
        m_SelectedFilePath = selectedItems.at(0)->data(0, Qt::UserRole+2).toString();
    } else {
        ui->txtFileName->setText(QString());
    }
}

void RemoteFileDialog::on_btnPathUp_clicked()
{
    QString path(ui->txtPath->text());

    QStringList directories = path.split(QLatin1Char('/'), QString::SkipEmptyParts);
    QString newPath(QLatin1Char('/'));
    foreach(QString directory, directories) {
        if(!directory.isEmpty() && directory != directories.last()) {
            newPath += directory + QLatin1Char('/');
        }
    }

    setPath(newPath);
}


} // namespace OpenSpeedShop
} // namespace Plugins

