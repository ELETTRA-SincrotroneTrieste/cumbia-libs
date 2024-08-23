#-------------------------------------------------
#
# Project created by QtCreator 2018-10-30T11:01:05
#
#-------------------------------------------------
include(../qumbia-plugins.pri)

include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

exists($${INSTALL_ROOT}/include/cumbia-tango) {
    INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-tango
    LIBS += -lcumbia-tango
} else {
    error("cumbia-tango installation not found under $${INSTALL_ROOT}")
}

# INSTALL_ROOT is used to install the target
# prefix is used within DEFINES +=
#
# cumbia installation script uses a temporary INSTALL_ROOT during build
# and then files are copied into the destination prefix. That's where
# configuration files must be found by the application when the script
# installs everything at destination
#
isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES_QUMBIA_PLUGINS_LIBDIR = $${prefix}/lib/qumbia-plugins

# added by cumbia-qtcontrols.pri, not included by this file
#
DEFINES += \
    CUMBIA_QTCONTROLS_PLUGIN_DIR=\"\\\"$${DEFINES_QUMBIA_PLUGINS_LIBDIR}\\\"\" \

QT       += core gui

isEmpty(buildtype) {
        buildtype = release
} else {
    equals(buildtype, debug) {
        message("")
        message("debug build")
        message("")
    }
}

CONFIG += $${buildtype}

TARGET = widgets-std-context-menu-actions
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
        widgetstdcontextmenuactions.cpp

HEADERS += \
        widgetstdcontextmenuactions.h

DISTFILES += widgets-std-context-menu-actions.json

inc.files = $${HEADERS}


# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
