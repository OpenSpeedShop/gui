# This file is part of the Open|SpeedShop Graphical User Interface
# Copyright (C) 2010-2013 Argo Navis Technologies, LLC
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

CONFIG(debug, debug|release) {
  TARGET           = ServerConnectionD
} else {
  TARGET           = ServerConnection
}

SOURCES           += ServerConnectionPlugin.cpp \
                     ServerConnection.cpp \
                     ServerConnectionPage.cpp

HEADERS           += ServerConnectionPlugin.h \
                     ServerConnection.h \
                     ServerConnectionPage.h

FORMS             += ServerConnectionPage.ui

RESOURCES         +=

QT                += network xml

LIBS        += -L$$quote($${BUILD_PATH}/plugins/OpenSpeedShop/$${DIR_POSTFIX}) -lOpenSpeedShop$${LIB_POSTFIX}

#debug: DEFINES += DIRECTCONNECTION_DEBUG

#serverConnectionHeaders.path = /include/plugins/ServerConnection
#serverConnectionHeaders.files = ServerConnectionLibrary.h ServerConnection.h
#INSTALLS += serverConnectionHeaders
