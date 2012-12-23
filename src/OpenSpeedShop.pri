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

!isEmpty(OPENSPEEDSHOP_PRI_INCLUDED): error("OpenSpeedShop.pri already included")
OPENSPEEDSHOP_PRI_INCLUDED = 1

##################
# QT INFORMATION #
##################
defineTest(qtVer) {
    isEqual(QT_MAJOR_VERSION, $$1) {
        isEqual(QT_MINOR_VERSION, $$2) {
            isEqual(QT_PATCH_VERSION, $$3) : return(true)
            greaterThan(QT_PATCH_VERSION, $$3) : return(true)
        }
        greaterThan(QT_MINOR_VERSION, $$2) : return(true)
    }
    return(false)
}
!qtVer(4,6,0): error(This application requires at least Qt version 4.6.0)

#####################
# QMAKE INFORMATION #
#####################
CONFIG -= debug_and_release
QT += core gui

#######################
# VERSION INFORMATION #
#######################
VER_MAJ      = 0
VER_MIN      = 3
VER_PAT      = 0
VERSION      = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
DEFINES     += VER_MAJ VER_MIN VER_PAT VERSION

#################
# INSTALL PATHS #
#################
!isEmpty(SOURCEDIR): SOURCE_PATH = $${SOURCEDIR}
isEmpty(SOURCE_PATH): SOURCE_PATH = $$quote($${PWD})

!isEmpty(BUILDDIR): BUILD_PATH = $${BUILDDIR}
isEmpty(BUILD_PATH): BUILD_PATH = $$quote($${PWD})

#########################
# DEBUG/RELEASE DIR_POSTFIX #
#########################
CONFIG(debug_and_release) {
  CONFIG(debug, debug|release) {
    DIR_POSTFIX = debug
  } else {
    DIR_POSTFIX = release
  }
}

CONFIG(debug, debug|release) {
  win32: LIB_POSTFIX = D$${VER_MAJ}
  else: LIB_POSTFIX = D
} else {
  win32: LIB_POSTFIX = $${VER_MAJ}
  else: LIB_POSTFIX =
}

#################
# INCLUDE PATHS #
#################
INCLUDEPATH       += $$quote($${SOURCE_PATH}/plugins)
DEPENDPATH        += $$quote($${SOURCE_PATH}/plugins)
