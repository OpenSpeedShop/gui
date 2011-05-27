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
                ModelManager/DataModel.cpp \
                ConnectionManager/ServerAdapter.cpp \
                RemoteFileSystem/RemoteFileEngine.cpp \
                RemoteFileSystem/RemoteFileEngineHandler.cpp \
                ModelManager/ModelDescriptor.cpp \
                ModelManager/ModelDescriptorWidget.cpp \
                ModelManager/ModelManager.cpp \
                ModelManager/ModelManagerDialog.cpp \
                ModelManager/ModelDescriptorListWidget.cpp


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
                ModelManager/DataModel.h \
                ConnectionManager/ConnectionManagerLibrary.h \
                OpenSpeedShopLibrary.h \
                ConnectionManager/ServerAdapter.h \
                RemoteFileSystem/RemoteFileEngine.h \
                RemoteFileSystem/RemoteFileEngineHandler.h \
                ModelManager/ModelDescriptor.h \
                ModelManager/ModelDescriptorWidget.h \
                ModelManager/ModelManager.h \
                ModelManager/ModelManagerDialog.h \
                ModelManager/ModelDescriptorListWidget.h

FORMS        += Settings/SettingPage.ui \
                AboutDialog.ui \
                Experiment/ExperimentWidget.ui \
                OpenSpeedShopWidget.ui \
                ConnectionManager/ConnectionWidget.ui \
                ModelManager/ModelDescriptorWidget.ui \
                ModelManager/ModelManagerDialog.ui \
                ModelManager/ModelDescriptorListWidget.ui

RESOURCES    += Resources/OpenSpeedShop.qrc

QT           += xml

DEFINES      += OPENSPEEDSHOP_LIBRARY \
                CONNECTIONMANAGER_LIBRARY

#debug: DEFINES += DATAMODEL_DEBUG
#debug: DEFINES += MODELMANAGER_DEBUG

# List of experiment types and their possible modifiers/metrics
# In the future, this should be pulled from the server
OTHER_FILES += ConnectionManager/ExperimentTypes.xml
experimentTypes.path = /bin
experimentTypes.files = ConnectionManager/ExperimentTypes.xml
INSTALLS += experimentTypes

# Default data for the model descriptors
# In the future, this should be pulled from the server
OTHER_FILES += ModelManager/ModelDescriptors.xml
modelDescriptors.path = /bin
modelDescriptors.files = ModelManager/ModelDescriptors.xml
INSTALLS += modelDescriptors

