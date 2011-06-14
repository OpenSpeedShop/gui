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

#include "SourceView.h"

#include <QPainter>
#include <QTextBlock>

namespace Plugins {
namespace OpenSpeedShop {

SourceView::SourceView(QWidget *parent) :
    QPlainTextEdit(parent)
{
    m_SideBarArea = new SideBarArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSideBarAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateSideBarArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateSideBarAreaWidth(0);
    highlightCurrentLine();
}

int SourceView::sideBarAreaWidth()
{
    int digits = 2;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void SourceView::updateSideBarAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(sideBarAreaWidth(), 0, 0, 0);
}

void SourceView::updateSideBarArea(const QRect &rect, int dy)
{
    if (dy)
        m_SideBarArea->scroll(0, dy);
    else
        m_SideBarArea->update(0, rect.y(), m_SideBarArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateSideBarAreaWidth(0);
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
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

} // namespace OpenSpeedShop
} // namespace Plugins
