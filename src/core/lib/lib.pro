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

include(lib.pri)
TARGET   = Core

HEADERS += ActionManager/ActionManager.h \
           MainWindow/MainWindow.h \
           PluginManager/PluginManager.h \
           PluginManager/IPlugin.h \
           PluginManager/PluginWrapper.h \
           SettingManager/SettingManager.h \
           SettingManager/SettingDialog.h \
           SettingManager/ISettingPageFactory.h \
           SettingManager/ISettingPage.h \
           ActionManager/ActionItem.h \
           ActionManager/MenuItem.h \
    PluginManager/SettingPageFactory.h \
    PluginManager/SettingPage.h

SOURCES += ActionManager/ActionManager.cpp \
           MainWindow/MainWindow.cpp \
    	   PluginManager/PluginManager.cpp \
           PluginManager/IPlugin.cpp \
           PluginManager/PluginWrapper.cpp \
           SettingManager/SettingManager.cpp \
           SettingManager/SettingDialog.cpp \
           SettingManager/ISettingPageFactory.cpp \
           SettingManager/ISettingPage.cpp \
           ActionManager/ActionItem.cpp \
           ActionManager/MenuItem.cpp \
    PluginManager/SettingPageFactory.cpp \
    PluginManager/SettingPage.cpp

FORMS   += \
           SettingManager/SettingDialog.ui \
    PluginManager/SettingPage.ui

LIBS    += 

RESOURCES += Resources/Core.qrc

