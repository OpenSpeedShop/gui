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

#ifndef REMOTEFILEDIALOG_H
#define REMOTEFILEDIALOG_H

#include <QtCore>
#include <QtGui>
#include <QtXml>

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui {
    class RemoteFileDialog;
}

class RemoteFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteFileDialog(QWidget *parent = 0);
    ~RemoteFileDialog();

    QString path() const;
    void setPath(const QString &path);

    QString filter() const;
    void setFilter(const QString &filter);

    QString selectedFilePath() const;

protected slots:
    void on_txtPath_editingFinished();
    void on_treeWidget_activated(QModelIndex);
    void on_treeWidget_itemSelectionChanged();
    void on_btnPathUp_clicked();

private:
    Ui::RemoteFileDialog *ui;
    QString m_SelectedFilePath;

    QRegExp m_Filter;

};


} // namespace OpenSpeedShop
} // namespace Plugins

#endif // REMOTEFILEDIALOG_H
