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
include(../../Serene.pri)

isEmpty(SERENE_PATH) {
#
#  NOTE: Serene has been deprecated;
#        we'll no longer warn that it's missing, and
#        the code will eventually be removed completely
#
#  warning()
#  warning("The LineGraphView plugin has been disabled")
#  warning("The plugin depends on headers from the Serene library")
#  warning("To enable it, pass 'SERENE_PATH=~/src/Serene' to qmake, where ~/src/Serene is the base source directory of Serene")

} else {
  CONFIG(debug, debug|release) {
    TARGET           = LineGraphViewD
  } else {
    TARGET           = LineGraphView
  }

  SOURCES           += LineGraphViewPlugin.cpp \
                       LineGraphView.cpp

  HEADERS           += LineGraphViewPlugin.h \
                       LineGraphView.h

  FORMS             +=

  RESOURCES         +=

  LIBS              += -L$$quote($${BUILD_PATH}/plugins/OpenSpeedShop/$${DIR_POSTFIX}) -lOpenSpeedShop$${LIB_POSTFIX}

  #debug: DEFINES += LINEGRAPHVIEW_DEBUG

  #lineGraphViewHeaders.path = /include/plugins/LineGraphView
  #lineGraphViewHeaders.files = LineGraphViewLibrary.h LineGraphView.h
  #INSTALLS += lineGraphViewHeaders
}

