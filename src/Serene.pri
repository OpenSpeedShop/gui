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

!isEmpty(SERENE_PRI_INCLUDED):error("Serene.pri already included")
SERENE_PRI_INCLUDED = 1

#################
# CONFIGURATION #
#################
QT                += opengl

#######################
# VERSION INFORMATION #
#######################
VER_SERENE   = 0

###########################
# LIBRARY & INCLUDE PATHS #
###########################
isEmpty(SERENE_PATH) {
  warning()
  warning("The SERENE_PATH variable must be set to the location of the Serene")
  warning("library in order to build the advanced views.  See")
  warning("<https://github.com/OpenSpeedShop/Serene/wiki/Installation>")
  warning("for more information on downloading and building the Serene libraries.")

} else {
  !exists($$quote($${SERENE_PATH}/lib/lib.pro)) {
    warning("The plugin depends on headers from the Serene library.")
    warning("However, the path passed to qmake through 'SERENE_PATH=$$SERENE_PATH', doesn't contain the expected source files.")
    error("Source files at $$quote($${SERENE_PATH}/lib/) were not found.")
  }

  INCLUDEPATH += $$quote($${SERENE_PATH}/lib)
  DEPENDPATH  += $$quote($${SERENE_PATH}/lib)

  isEmpty(SERENE_LIBPATH): SERENE_LIBPATH = $${SERENE_PATH}

  SERENE_DIRECTORIES = $${SERENE_LIBPATH}
  SERENE_DIRECTORIES += $${SERENE_LIBPATH}/lib
  SERENE_DIRECTORIES += $${SERENE_LIBPATH}/debug
  SERENE_DIRECTORIES += $${SERENE_LIBPATH}/lib/debug
  SERENE_DIRECTORIES += $${SERENE_LIBPATH}/release
  SERENE_DIRECTORIES += $${SERENE_LIBPATH}/lib/release

  CONFIG(debug, debug|release) {
    win32 {
      SERENE_LIBNAME = SereneD0
      SERENE_FILENAME = $${SERENE_LIBNAME}.dll
    } else {
      SERENE_LIBNAME = SereneD
      SERENE_FILENAME = lib$${SERENE_LIBNAME}.so
    }
  } else {
    win32 {
      SERENE_LIBNAME = Serene0
      SERENE_FILENAME = $${SERENE_LIBNAME}.dll
    } else {
      SERENE_LIBNAME = Serene
      SERENE_FILENAME = lib$${SERENE_LIBNAME}.so
    }
  }

  for(SERENE_DIRECTORY, SERENE_DIRECTORIES) {
    exists($${SERENE_DIRECTORY}/$${SERENE_FILENAME}) {
      LIBS += -L$$quote($${SERENE_DIRECTORY}) -l$${SERENE_LIBNAME}
      unset(SERENE_DIRECTORIES)
      break()
    }
  }

  !isEmpty(SERENE_DIRECTORIES) {
    for(SERENE_DIRECTORY, SERENE_DIRECTORIES) {
      warning("$${SERENE_DIRECTORY}/$${SERENE_FILENAME} was not found")
    }
    error("Please ensure that you have already built the Serene library.")
  }

}
