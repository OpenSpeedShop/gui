/*!
   \file main.cpp
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

/*! \mainpage Open|SpeedShop GUI
    \author Dane Gardner <dane.gardner@gmail.com>
    \version v0.1 (dev)

    \section intro Introduction
    \par
    The Open|SpeedShop GUI project is a multi-platform graphical-user-interface for
    the Open|SpeedShop framework using the QT4 toolkit, and is currently under
    development by Argo Navis Technologies LLC under a SBIR grant from NASA.
    \par
    For those who are using this documentation to learn the project's development
    process, you might want to start with the main GUI object OpenSpeedShop.

    \section releases Releases
    \par
    None yet.

    \section roadmap Roadmap

    \par Development Staging and Milestones
    <ol>
      <li><b>Phase I</b>
        <ol>
          <li>Design GUI</li>
          <li>Build GUI Alpha/Beta</li>
          <li>Test GUI Beta</li>
        </ol>
      </li>
      <li><b>Phase II</b>
        <ol>
          <li>Work toward release version</li>
        </ol>
      </li>
      <li><b>Phase III</b>
        <ol>
          <li>???</li>
          <li>Profit</li>
        </ol>
      </li>
    </ol>

    \par Governing Design Ideas
    Intended features and design goals to be ubiquitous throughout the project
    <ul>
        <li>Tab based multi-document viewing that allows drag and drop capabilities to 'panes',
            with nested pane capablities</li>
        <li>Plugin modules that are dynamically loaded from a plugin manager, following QT4's
            plugin system designs.  This system should automatically load all available plugins
            and allow the user to select which to use, or automagically decide which fits the
            situation</li>
        <li>Allow for multi-platform use (Unix, Win32 and OSX) through remote control of the
            openss process via SSH and the CLI interface.  Or, for speed, allow local control
            through streaming directly to the stdin on openss.</li>
        <li>Attempt to keep library inclusion to a minimum.  Possibly adding zero libraries on top
            of just the QtCore, QtGui and QtSql set.  This will simplify installation, and
            keep the cross-platform goals easier to attain.</li>
    </ul>

    \section copyright Copyright
    \par
    This file is part of the Open|SpeedShop Graphical User Interface
    Copyright (C) 2010-2011 Argo Navis Technologies, LLC
    \par
    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2.1 of the License, or (at your
    option) any later version.
    \par
    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.
    \par
    You should have received a copy of the GNU Lesser General Public License
    along with this library; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */


#include <QtGui/QApplication>
#include <MainWindow/MainWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("OpenSpeedShop");
    QCoreApplication::setOrganizationDomain("openspeedshop.org");
    QCoreApplication::setApplicationName("OpenSpeedShop GUI");
    QCoreApplication::setApplicationVersion("0.1");

    Core::MainWindow::instance()->show();

    return a.exec();
}
