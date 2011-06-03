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

QT += core gui

APPLICATION_TARGET = OpenSpeedShop


SOURCE_PATH = $${SOURCEDIR}
isEmpty(SOURCE_PATH): SOURCE_PATH = $$quote($${PWD})

BUILD_PATH = $${BUILDDIR}
isEmpty(BUILD_PATH): BUILD_PATH = $$quote($${PWD})

win32 {
    release: POSTFIX = $$quote(release)
    debug:   POSTFIX = $$quote(debug)
}


message()
message(PWD: $$PWD)
message(OUT_PWD: $$OUT_PWD)
message()

#message(SOURCEDIR: $${SOURCEDIR})
#message(SOURCE_PATH: $${SOURCE_PATH})
#message(BUILDDIR: $$[BUILDDIR])
#message(BUILD_PATH: $${BUILD_PATH})
#message(POSTFIX: $${POSTFIX})


INCLUDEPATH       += $$quote($${SOURCE_PATH}/core/lib) $$quote($${SOURCE_PATH}/plugins)
DEPENDPATH        += $$quote($${SOURCE_PATH}/core/lib) $$quote($${SOURCE_PATH}/plugins)
