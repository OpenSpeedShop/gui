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

#include <QRegExp>
#include <QDomDocument>
#include <ConnectionManager/IAdapter.h>
#include <ConnectionManager/ConnectionManager.h>

namespace Plugins {
namespace OpenSpeedShop {

RemoteFileDialog::RemoteFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteFileDialog)
{
    ui->setupUi(this);
    setPath("/");
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

QString RemoteFileDialog::selectedFilePath() const
{
    return m_SelectedFilePath;
}


void RemoteFileDialog::on_txtPath_editingFinished()
{
    /* Connect to the server, and get the listing */
    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    QDomDocument document("Listing");
    document.setContent(adapter->waitDirStat(ui->txtPath->text()));

    QDomElement responseElement = document.firstChildElement("Response");
    if(responseElement.isNull()) throw tr("'Response' element doesn't exist, as expected.");

    QDomElement fileSystemElement = responseElement.firstChildElement("FileSystem");
    if(fileSystemElement.isNull()) throw tr("'FileSystem' element doesn't exist, as expected.");

    QDomElement directoryElement = fileSystemElement.firstChildElement("Dir");
    if(directoryElement.isNull()) {

        // Deal with other responses, like errors
        QDomElement exceptionElement = fileSystemElement.firstChildElement("Exception");
        if(!exceptionElement.isNull()) {
            throw tr("Exception returned from server: '%1'").arg(exceptionElement.text());
        }

        throw tr("'Dir' element doesn't exist, as expected. No error was given from the server.");
    }

    ui->treeWidget->clear();
    QDomElement fileElement = directoryElement.firstChildElement("File");
    while(!fileElement.isNull()) {
        QString fileName = fileElement.attribute("name");
        if(fileName.startsWith(QLatin1Char('.'))) {
            fileElement = fileElement.nextSiblingElement(fileElement.tagName());
            continue;
        }

        QTreeWidgetItem *fileItem = new QTreeWidgetItem(ui->treeWidget);
        fileItem->setText(0, fileName);

        QString type(fileElement.attribute("type"));
        if(!type.compare("file", Qt::CaseInsensitive)) {
            fileItem->setIcon(0, QIcon(":/OpenSpeedShop/file.png"));

            //HACK:
            if(!fileName.endsWith(".openss")) {
                fileItem->setDisabled(true);
            }

        } else if(!type.compare("directory", Qt::CaseInsensitive)) {
            fileItem->setIcon(0, QIcon(":/OpenSpeedShop/folder.svg"));
        } else {
            fileItem->setIcon(0, QIcon());
        }
        fileItem->setData(0, Qt::UserRole, type);

        QString path = fileElement.attribute("path");
        if(!path.endsWith(QLatin1Char('/'))) {
            path.append(QLatin1Char('/'));
        }
        fileItem->setData(0, Qt::UserRole+1, fileElement.attribute("path"));

        fileItem->setData(0, Qt::UserRole+2, path + fileName);

        ui->treeWidget->addTopLevelItem(fileItem);

        fileElement = fileElement.nextSiblingElement(fileElement.tagName());
    }

    ui->treeWidget->sortItems(0, Qt::AscendingOrder);
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

