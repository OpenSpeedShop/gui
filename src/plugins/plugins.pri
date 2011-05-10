include(../OpenSpeedShop.pri)

TEMPLATE      = lib
CONFIG       += plugin plugin_with_soname
CONFIG       += depend_includepath

LIBS         += -L$$quote(${BUILDDIR}/core/lib/debug) -lCore

target.path   = /lib/plugins
INSTALLS     += target
