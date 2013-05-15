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

#include "SourceView.h"

#include <QPainter>
#include <QIcon>
#include <QToolTip>

namespace Plugins {
namespace OpenSpeedShop {

SourceView::SourceView(QWidget *parent) :
    QPlainTextEdit(parent),
    m_SideBarArea(new SideBarArea(this)),
    m_SyntaxHighlighter(this->document())
{
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSideBarAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateSideBarArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    setFont(font);

    updateSideBarAreaWidth(0);
    highlightCurrentLine();
}

void SourceView::setCurrentLineNumber(const int &lineNumber)
{
    const QTextBlock &block = document()->findBlockByNumber(lineNumber-1);
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 0);
    setTextCursor(cursor);
    centerCursor();
    setFocus();
}

int SourceView::sideBarAreaWidth()
{
    int digits = QString::number(qMax(1, blockCount())).length();
    int digitWidth = fontMetrics().width(QLatin1Char('9')) * digits;

    int diameter = fontMetrics().height();

    return digitWidth + diameter + 3;
}

void SourceView::updateSideBarAreaWidth(int newBlockCount)
{
    Q_UNUSED(newBlockCount)
    setViewportMargins(sideBarAreaWidth(), 0, 0, 0);
}

void SourceView::updateSideBarArea(const QRect &rect, int dy)
{
    if(dy) {
        m_SideBarArea->scroll(0, dy);
    } else {
        m_SideBarArea->update(0, rect.y(), m_SideBarArea->width(), rect.height());
    }

    if(rect.contains(viewport()->rect())) {
        updateSideBarAreaWidth(0);
    }
}

void SourceView::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_SideBarArea->setGeometry(QRect(cr.left(), cr.top(), sideBarAreaWidth(), cr.height()));
}

void SourceView::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::yellow).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}

void SourceView::sideBarAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_SideBarArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::darkGray);

            painter.drawText(0, top, m_SideBarArea->width(), fontMetrics().height(), Qt::AlignRight, number);

            if(m_Statements.contains(blockNumber + 1)) {
                int diameter = fontMetrics().height();
                QIcon icon(":/OpenSpeedShop/sourceMarker.svg");
                QPixmap pixmap = icon.pixmap(QSize(diameter, diameter));
                painter.drawPixmap(0,top, pixmap.height(), pixmap.width(), pixmap);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void SourceView::setModel(QAbstractItemModel *model)
{
    m_Model = model;
    refreshStatements();
}

void SourceView::refreshStatements()
{
    m_Statements.clear();
    if(m_FilePath.isEmpty() || !m_Model) {
        return;
    }

    int statementColumn = -1;
    QString statementType;
    for(int i=0; i < m_Model->columnCount(); ++i) {
        QString columnType = m_Model->headerData(i, Qt::Horizontal, Qt::ToolTipRole).toString();
        if(columnType == "Statement" || columnType == "Function") {
            statementType = columnType;
            statementColumn = i;
            break;
        }
    }

    if(statementColumn >= 0) {
        QRegExp statementPattern;
        if(statementType == "Function") {
            statementPattern.setPattern("^.*\\((.*):([0-9]+)\\)$");
        } else if(statementType == "Statement") {
            statementPattern.setPattern("^(.*):([0-9]+)$");
        }

        for(int i=0; i < m_Model->rowCount(); ++i) {
            QModelIndex index = m_Model->index(i, statementColumn);
            QString statementText = index.data(Qt::DisplayRole).toString();

            if(statementPattern.exactMatch(statementText)) {
                QString filePath = statementPattern.cap(1);

                bool okay;
                int lineNumber = statementPattern.cap(2).toInt(&okay);
                if(!okay) { lineNumber = 0; }

                if(filePath == m_FilePath) {
                    m_Statements.insert(lineNumber, index);
                }
            }

        }
    }
}

void SourceView::setFilePath(const QString &filePath)
{
    m_FilePath = filePath;
    refreshStatements();
}

bool SourceView::event(QEvent *event)
{
    if(event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        QPoint position = helpEvent->pos();
        if(position.x() > sideBarAreaWidth()) {
            QToolTip::hideText();
            event->ignore();
            return true;
        }

        QTextCursor cursor = cursorForPosition(position);
        int lineNumber = cursor.blockNumber() + 1;
        if(!m_Statements.contains(lineNumber)) {
            QToolTip::hideText();
            event->ignore();
            return true;
        }

        QModelIndex rowIndex = m_Statements.value(lineNumber);
        const QAbstractItemModel *model = rowIndex.model();

        QStringList toolTip;
        for(int i=0; i < model->columnCount(); ++i) {
            QModelIndex index = model->index(rowIndex.row(), i, rowIndex.parent());
            QString header = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
            QString value = index.data(Qt::DisplayRole).toString();
            toolTip.append(header + QLatin1String(":  ") + value);
        }

        QToolTip::showText(helpEvent->globalPos(), toolTip.join(QLatin1String("\n")));
        return true;
    }

    return QPlainTextEdit::event(event);
}


} // namespace OpenSpeedShop
} // namespace Plugins
