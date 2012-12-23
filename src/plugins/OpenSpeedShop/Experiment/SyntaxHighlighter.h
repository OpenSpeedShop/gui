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

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QtCore>
#include <QtGui>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QObject *parent = 0);
    explicit SyntaxHighlighter(QTextDocument *parent);
    explicit SyntaxHighlighter(QTextEdit *parent);

    void init();

    void highlightBlock(const QString &text);

private:
    QRegExp m_Keywords;
    QRegExp m_DataTypes;

    enum States {
        State_NormalState = -1,
        State_InsideComment,
        State_InsideDoubleQuote,
        State_InsideSingleQuote,
        State_InsideAngleBracketQuote
    };

};

#endif // SYNTAXHIGHLIGHTER_H
