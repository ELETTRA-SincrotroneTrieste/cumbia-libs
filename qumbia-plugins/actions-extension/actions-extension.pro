include(../qumbia-plugins.pri)

# configuration files must be found by the application when the script
# installs everything at destination
#
exists($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
    message("including support for qumbia-epics-controls module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
}

exists($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
    message("including support for qumbia-tango-controls module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
}

include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

QT       += core gui

TARGET = actions-extension-plugin
TEMPLATE = lib
CONFIG += plugin debug

CONFIG += silent

SOURCES += \
    actionextensions.cpp \
    gettdbpropertyextension.cpp \
    actionextensionfactory.cpp \
    infodialogextension.cpp \
    cuinfodialog.cpp \
    cuapplicationlauncherextension.cpp

HEADERS += \
    actionextensions.h \
    gettdbpropertyextension.h \
    actionextensionfactory.h \
    infodialogextension.h \
    cuinfodialog.h \
    cuapplicationlauncherextension.h
DISTFILES += actions-extension.json 

inc.files += $${HEADERS}

# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
