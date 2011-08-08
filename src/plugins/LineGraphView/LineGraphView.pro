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
TARGET             = LineGraphView

SOURCES           += LineGraphViewPlugin.cpp \
                     LineGraphView.cpp \
                     Delegate.cpp

HEADERS           += LineGraphViewPlugin.h \
                     LineGraphView.h \
                     Delegate.h

FORMS             +=

RESOURCES         +=

LIBS              += -L$$quote($${BUILD_PATH}/plugins/OpenSpeedShop/$${POSTFIX}) -lOpenSpeedShop

# Include the Serene library items to the project
QT                += OpenGL
LIBS              += -L$$quote($${BUILD_PATH}/Serene/lib/$${POSTFIX}) -lSerene
INCLUDEPATH       += $$quote($${SOURCE_PATH}/Serene/lib)
DEPENDPATH        += $$quote($${SOURCE_PATH}/Serene/lib)

#debug: DEFINES += LINEGRAPHVIEW_DEBUG
