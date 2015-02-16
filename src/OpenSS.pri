# This file is part of the Open|SpeedShop Graphical User Interface
# Copyright (C) 2010-2015 Argo Navis Technologies, LLC
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

!isEmpty(OPENSS_PRI_INCLUDED):error("OpenSpeedShop.pri already included")
OPENSS_PRI_INCLUDED = 1

###########################
# LIBRARY & INCLUDE PATHS #
###########################
!win32 {
  isEmpty(OPENSS_PATH) {
    warning()
    warning("The OPENSS_PATH variable must be set to the location of the Open|SpeedShop framework in order to " \
            "build the direct connection plugin.  See <https://www.openspeedshop.org/> for more information on " \
            "downloading and building the Open|SpeedShop framework.")
    warning()

  } else {
    !exists($$quote($${OPENSS_PATH}/include/openspeedshop/Direct.hxx)) {
      warning()
      warning("The plugin depends on headers from the Open|SpeedShop tool framework.  However, the path passed " \
              "to qmake through 'OPENSS_PATH=$$OPENSS_PATH', doesn't contain the expected source files.")
      error("Source files at $$quote($${OPENSS_PATH}/include/) were not found.")
    }

    INCLUDEPATH += $$quote($${OPENSS_PATH}/include)
    DEPENDPATH  += $$quote($${OPENSS_PATH}/include)

    isEmpty(OPENSS_LIBPATH): OPENSS_LIBPATH = $${OPENSS_PATH}

    OPENSS_DIRECTORIES = $${OPENSS_LIBPATH}
    OPENSS_DIRECTORIES += $${OPENSS_LIBPATH}/lib
    OPENSS_DIRECTORIES += $${OPENSS_LIBPATH}/lib64

    OPENSS_LIBFILE = openss-cli-direct
    OPENSS_FILENAME = lib$${OPENSS_LIBFILE}.so

    for(OPENSS_DIRECTORY, OPENSS_DIRECTORIES) {
      exists($${OPENSS_DIRECTORY}/$${OPENSS_FILENAME}) {
        LIBS += -L$$quote($${OPENSS_DIRECTORY}) -l$${OPENSS_LIBFILE}
        unset(OPENSS_DIRECTORIES)
        break()
      }
    }

    !isEmpty(OPENSS_DIRECTORIES) {
      for(OPENSS_DIRECTORY, OPENSS_DIRECTORIES) {
        warning("$${OPENSS_DIRECTORY}/$${OPENSS_FILENAME} was not found")
      }
      error("Please ensure that you have already built the Open|SpeedShop tool framework.")
    }

  }
}
