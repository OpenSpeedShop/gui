# This file is part of the Open|SpeedShop Graphical User Interface
# Copyright (C) 2010-2011 Argo Navis Technologies, LLC
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

include(../plugins.pri)

TARGET             = DirectConnection

SOURCES           += DirectConnectionPlugin.cpp \
                     DirectConnection.cpp \
                     DirectConnectionPage.cpp
             

HEADERS           += DirectConnectionPlugin.h \
                     DirectConnection.h \
                     DirectConnectionPage.h

FORMS             += DirectConnectionPage.ui

RESOURCES         +=

QT                += network xml

INCLUDEPATH       += $$quote($${SOURCE_PATH}/plugins/OpenSpeedShop)
DEPENDPATH        += $$quote($${SOURCE_PATH}/plugins/OpenSpeedShop)
LIBS              += -L$$quote(${BUILDDIR}/plugins/OpenSpeedShop/debug) -lOpenSpeedShop

#debug: DEFINES += DIRECTCONNECTION_DEBUG
