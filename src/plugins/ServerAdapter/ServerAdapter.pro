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

CONFIG(debug, debug|release) {
  TARGET            = ServerAdapterD
} else {
  TARGET            = ServerAdapter
}

SOURCES            += ServerAdapterPlugin.cpp \
                      ServerAdapter.cpp

HEADERS            += ServerAdapterPlugin.h \
                      ServerAdapter.h

QT                 += xml

LIBS         += -L$$quote($${BUILD_PATH}/plugins/OpenSpeedShop/$${DIR_POSTFIX}) -lOpenSpeedShop$${LIB_POSTFIX}

#debug: DEFINES += SERVERADAPTER_DEBUG


# List of experiment types and their possible modifiers/metrics
# In the future, this should be pulled from the server
OTHER_FILES += ExperimentTypes.xml
win32:experimentTypes.path = /oss/
else:experimentTypes.path = /share/openspeedshop/gui/
experimentTypes.files = ExperimentTypes.xml
INSTALLS += experimentTypes
