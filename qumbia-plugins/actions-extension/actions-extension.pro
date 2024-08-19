include(../qumbia-plugins.pri)s

exists($${INSTALL_ROOT}/include/quapps/quapps.pri) {
    include($${INSTALL_ROOT}/include/quapps/quapps.pri)
}

include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

QT       += core gui

TARGET = actions-extension-plugin
TEMPLATE = lib
CONFIG += plugin

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

SOURCES += \
    actionextensions.cpp \
    cuengineswitchdialog.cpp \
    gettdbpropertyextension.cpp \
    actionextensionfactory.cpp \
    infodialogextension.cpp \
    cuinfodialog.cpp \
    cuapplicationlauncherextension.cpp \
    engineswitchdialogextension.cpp

HEADERS += \
    actionextensions.h \
    cuengineswitchdialog.h \
    gettdbpropertyextension.h \
    actionextensionfactory.h \
    infodialogextension.h \
    cuinfodialog.h \
    cuapplicationlauncherextension.h \
    engineswitchdialogextension.h
DISTFILES += actions-extension.json 

inc.files += $${HEADERS}

# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
