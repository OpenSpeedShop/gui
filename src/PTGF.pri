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

!isEmpty(PTGF_PRI_INCLUDED): error("PTGF.pri already included")
PTGF_PRI_INCLUDED = 1

##############
# PTGF PATHS #
##############

# Source path for PTGF
isEmpty(PTGF_INSTALLROOT) {
  !isEmpty($$quote($${INSTALL_ROOT})): PTGF_INSTALLROOT = $$quote($${INSTALL_ROOT})
  !isEmpty($$quote($${PWD})): PTGF_INSTALLROOT = $$quote($${PWD})
}

######################
# PTGF INCLUDE PATHS #
######################

isEmpty(PTGF_INCPATH): PTGF_INCPATH = $$quote($${PTGF_INSTALLROOT})
PTGF_INCPATHS = $${PTGF_INCPATH}
PTGF_INCPATHS = $${PTGF_INCPATH}/src         # /src/PTGF/src/ ...
PTGF_INCPATHS += $${PTGF_INCPATH}/include    # /opt/PTGF/include/ ...
PTGF_INCPATHS += $${PTGF_INCPATH}/PTGF       # /usr/include/PTGF/ ...

PTGF_FILENAME = core/lib/Global.h

# Loop through possible directories to find the header
for(PTGF_PATH, PTGF_INCPATHS) {
  exists($${PTGF_PATH}/$${PTGF_FILENAME}) {
    INCLUDEPATH += $$quote($${PTGF_PATH}/core/lib) $$quote($${PTGF_PATH}/plugins)
    DEPENDPATH  += $$quote($${PTGF_PATH}/core/lib) $$quote($${PTGF_PATH}/plugins)
    unset(PTGF_INCPATHS)
    break()
  }
}


!isEmpty(PTGF_INCPATHS) {
  warning()
  for(PTGF_PATH, PTGF_INCPATHS) {
    warning("$${PTGF_PATH}/$${PTGF_FILENAME} was not found")
  }
  warning("The PTGF header files could not be found.")
  warning("The SWAT plugin requires access to the framework source.")
  warning("Pass 'PTGF_INCPATH=~/src/PTGF' to qmake, where ~/src/PTGF is the base source directory of PTGF.")
  warning("-=] OR [=-")
  error("Pass 'PTGF_INSTALLROOT=/opt/PTGF' to qmake, where /opt/PTGF is the base install directory of PTGF.")
}

######################
# PTGF LIBRARY PATHS #
######################

# Library path for compiled/built PTGF
isEmpty(PTGF_LIBPATH): PTGF_LIBPATH = $$quote($${PTGF_INSTALLROOT})

# Possible subdirectories for DLL
PTGF_LIBPATHS = $${PTGF_LIBPATH}
PTGF_LIBPATHS += $${PTGF_LIBPATH}/lib
PTGF_LIBPATHS += $${PTGF_LIBPATH}/debug
PTGF_LIBPATHS += $${PTGF_LIBPATH}/lib/debug
PTGF_LIBPATHS += $${PTGF_LIBPATH}/release
PTGF_LIBPATHS += $${PTGF_LIBPATH}/lib/release

# Find name of library file
CONFIG(debug, debug|release) {
  win32 {
    PTGF_LIBNAME = CoreD0
    PTGF_FILENAME = $${PTGF_LIBNAME}.dll
  } else {
    PTGF_LIBNAME = CoreD
    PTGF_FILENAME = lib$${PTGF_LIBNAME}.so
  }
} else {
  win32 {
    PTGF_LIBNAME = Core0
    PTGF_FILENAME = $${PTGF_LIBNAME}.dll
  } else {
    PTGF_LIBNAME = Core
    PTGF_FILENAME = lib$${PTGF_LIBNAME}.so
  }
}

# Loop through possible directories to find the DLL
for(PTGF_PATH, PTGF_LIBPATHS) {
  exists($${PTGF_PATH}/$${PTGF_FILENAME}) {
    LIBS += -L$$quote($${PTGF_PATH}) -l$${PTGF_LIBNAME}
    unset(PTGF_LIBPATHS)
    unset(PTGF_FILENAME)
    break()
  }
}

# If we never found an existing library file, notify the user, and error out
!isEmpty(PTGF_LIBPATHS) {
  warning()
  for(PTGF_PATH, PTGF_LIBPATHS) {
    warning("$${PTGF_PATH}/$${PTGF_FILENAME} was not found")
  }
  warning("The PTGF library files could not be found.")
  warning("The SWAT plugin requires access to the framework libraries.")
  warning("Please ensure that you have already built the PTGF Core library.")
  warning("Pass 'PTGF_LIBPATH=/opt/PTGF' to qmake, where /opt/PTGF is the base library install directory of PTGF.")
  warning("-=] OR [=-")
  error("Pass 'PTGF_INSTALLROOT=/opt/PTGF' to qmake, where /opt/PTGF is the base install directory of PTGF.")
}
