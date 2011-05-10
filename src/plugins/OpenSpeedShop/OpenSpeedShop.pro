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

TARGET        = OpenSpeedShop

SOURCES      += OpenSpeedShopPlugin.cpp \
                Settings/SettingPageFactory.cpp \
                Settings/SettingPage.cpp \
                AboutDialog.cpp \
                Experiment/ExperimentWidget.cpp \
                OpenSpeedShopWidget.cpp \
                ConnectionManager/ServerCommand.cpp \
                ConnectionManager/IConnection.cpp \
                ConnectionManager/ConnectionWidget.cpp \
                ConnectionManager/ConnectionManager.cpp \
                ViewManager/DataModel.cpp \
                ViewManager/DataItem.cpp \
                ConnectionManager/ServerAdapter.cpp

HEADERS      += OpenSpeedShopPlugin.h \
                Settings/SettingPageFactory.h \
                Settings/SettingPage.h \
                AboutDialog.h \
                Experiment/ExperimentWidget.h \
                OpenSpeedShopWidget.h \
                ConnectionManager/ServerCommand.h \
                ConnectionManager/IConnection.h \
                ConnectionManager/ConnectionWidget.h \
                ConnectionManager/ConnectionManager.h \
                ViewManager/DataModel.h \
                ViewManager/DataItem.h \
                ConnectionManager/ConnectionManagerLibrary.h \
                OpenSpeedShopLibrary.h \
                ConnectionManager/ServerAdapter.h

FORMS        += Settings/SettingPage.ui \
                AboutDialog.ui \
                Experiment/ExperimentWidget.ui \
                OpenSpeedShopWidget.ui \
                ConnectionManager/ConnectionWidget.ui

RESOURCES    += Resources/OpenSpeedShop.qrc

QT           += xml

DEFINES      += OPENSPEEDSHOP_LIBRARY \
                CONNECTIONMANAGER_LIBRARY
