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

#ifndef SOURCEVIEW_H
#define SOURCEVIEW_H

#include <QtCore>
#include <QtGui>

#include "SyntaxHighlighter.h"

namespace Plugins {
namespace OpenSpeedShop {

class SourceView : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit SourceView(QWidget *parent = 0);

    void setCurrentLineNumber(const int &lineNumber);

    void setModel(QAbstractItemModel *model);
    void setFilePath(const QString &filePath);

protected:
    void resizeEvent(QResizeEvent *event);
    void sideBarAreaPaintEvent(QPaintEvent *event);
    int sideBarAreaWidth();
    void refreshStatements();

    bool event(QEvent *event);

private slots:
    void updateSideBarAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateSideBarArea(const QRect &, int);

private:
    QWidget *m_SideBarArea;
    SyntaxHighlighter m_SyntaxHighlighter;

    QMap<int, QModelIndex> m_Statements;
    QAbstractItemModel *m_Model;
    QString m_FilePath;

    friend class SideBarArea;
};

class SideBarArea : public QWidget
{
public:
    SideBarArea(SourceView *sourceView) : QWidget(sourceView) { m_SourceView = sourceView; }
    QSize sizeHint() const { return QSize(m_SourceView->sideBarAreaWidth(), 0); }
protected:
    void paintEvent(QPaintEvent *event) { m_SourceView->sideBarAreaPaintEvent(event); }
private:
    SourceView *m_SourceView;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // SOURCEVIEW_H
