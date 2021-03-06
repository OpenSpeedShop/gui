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

include(../plugins.pri)

LIBS         += -L$$quote($${BUILD_PATH}/plugins/Welcome/$${DIR_POSTFIX}) -lWelcome$${LIB_POSTFIX}
LIBS         += -L$$quote($${BUILD_PATH}/plugins/NodeListView/$${DIR_POSTFIX}) -lNodeListView$${LIB_POSTFIX}

CONFIG(debug, debug|release) {
  TARGET      = OpenSpeedShopD
} else {
  TARGET      = OpenSpeedShop
}

SOURCES      += OpenSpeedShopPlugin.cpp \
                Settings/SettingPage.cpp \
                Experiment/ExperimentWidget.cpp \
                OpenSpeedShopWidget.cpp \
                ConnectionManager/ServerCommand.cpp \
                ConnectionManager/IConnection.cpp \
                ConnectionManager/ConnectionWidget.cpp \
                ConnectionManager/ConnectionManager.cpp \
                ModelManager/DataModel.cpp \
                ModelManager/FilterDescriptor.cpp \
                ModelManager/ModelDescriptor.cpp \
                ModelManager/ModelDescriptorWidget.cpp \
                ModelManager/ModelManager.cpp \
                ModelManager/ModelManagerDialog.cpp \
                ModelManager/ModelDescriptorListWidget.cpp \
                ConnectionManager/IAdapter.cpp \
                ConnectionManager/IConnectionPage.cpp \
                RemoteFileSystem/RemoteFileDialog.cpp \
                Experiment/SourceView.cpp \
                Experiment/SyntaxHighlighter.cpp \
                RemoteFileSystem/PathRewriter.cpp \
                Welcome/WelcomeData.cpp \
                AboutWidget.cpp \
                MainWindow.cpp

HEADERS      += OpenSpeedShopPlugin.h \
                Settings/SettingPage.h \
                Experiment/ExperimentWidget.h \
                OpenSpeedShopWidget.h \
                ConnectionManager/ServerCommand.h \
                ConnectionManager/IConnection.h \
                ConnectionManager/ConnectionWidget.h \
                ConnectionManager/ConnectionManager.h \
                ModelManager/DataModel.h \
                ConnectionManager/ConnectionManagerLibrary.h \
                OpenSpeedShopLibrary.h \
                ModelManager/FilterDescriptor.h \
                ModelManager/ModelDescriptor.h \
                ModelManager/ModelDescriptorWidget.h \
                ModelManager/ModelManager.h \
                ModelManager/ModelManagerDialog.h \
                ModelManager/ModelDescriptorListWidget.h \
                ConnectionManager/IAdapter.h \
                ConnectionManager/IConnectionPage.h \
                ModelManager/ModelManagerLibrary.h \
                RemoteFileSystem/RemoteFileDialog.h \
                Experiment/SourceView.h \
                Experiment/SyntaxHighlighter.h \
                Welcome/WelcomeData.h \
                RemoteFileSystem/PathRewriter.h \
                AboutWidget.h \
                MainWindow.h

FORMS        += Settings/SettingPage.ui \
                Experiment/ExperimentWidget.ui \
                OpenSpeedShopWidget.ui \
                ConnectionManager/ConnectionWidget.ui \
                ModelManager/ModelDescriptorWidget.ui \
                ModelManager/ModelManagerDialog.ui \
                RemoteFileSystem/RemoteFileDialog.ui \
                AboutWidget.ui

RESOURCES    += Resources/OpenSpeedShop.qrc

QT           += xml

DEFINES      += OPENSPEEDSHOP_LIBRARY \
                CONNECTIONMANAGER_LIBRARY \
                VIEWMANAGER_LIBRARY \
                MODELMANAGER_LIBRARY

#debug: DEFINES += DATAMODEL_DEBUG
#debug: DEFINES += MODELMANAGER_DEBUG
#debug: DEFINES += CONNECTIONMANAGER_DEBUG


# Default data for the model descriptors
# In the future, this should be pulled from the server
OTHER_FILES += ModelManager/ModelDescriptors.xml
win32:modelDescriptors.path = /oss/
else:modelDescriptors.path = /share/openspeedshop/gui/
modelDescriptors.files = ModelManager/ModelDescriptors.xml
INSTALLS += modelDescriptors

# Install the welcome data file
OTHER_FILES += Welcome/WelcomeData.xml
win32: welcomeData.path = /oss/
else: welcomeData.path = /share/openspeedshop/gui/
welcomeData.files = Welcome/WelcomeData.xml
INSTALLS += welcomeData

openSpeedShopHeaders.path = /include/plugins/OpenSpeedShop
openSpeedShopHeaders.files = OpenSpeedShopLibrary.h \
                             OpenSpeedShopWidget.h
INSTALLS += openSpeedShopHeaders

connectionManagerHeaders.path = /include/plugins/OpenSpeedShop/ConnectionManager
connectionManagerHeaders.files = ConnectionManager/ConnectionManagerLibrary.h \
                                 ConnectionManager/ConnectionManager.h \
                                 ConnectionManager/IAdapter.h \
                                 ConnectionManager/IConnection.h \
                                 ConnectionManagerIConnectionPage.h \
                                 ConnectionManager/ServerCommand.h
INSTALLS += connectionManagerHeaders

modelManagerHeaders.path = /include/plugins/OpenSpeedShop/ModelManager
modelManagerHeaders.files = ModelManager/ModelManagerLibrary.h \
                            ModelManager/ModelManager.h \
                            ModelManager/ModelDescriptor.h \
                            ModelManager/ModelDescriptorWidget.h \
                            ModelManager/ModelDescriptorListWidget.h
INSTALLS += modelManagerHeaders
