include(../OpenSpeedShop.pri)

TEMPLATE      = lib
CONFIG       += plugin plugin_with_soname
CONFIG       += depend_includepath

LIBS         += -L$$quote($${BUILD_PATH}/core/lib/$${POSTFIX}) -lCore

win32:target.path   = /
else:target.path   = /lib/plugins

INSTALLS     += target
