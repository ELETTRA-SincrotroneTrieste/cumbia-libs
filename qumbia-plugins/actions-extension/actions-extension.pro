#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T14:13:50
#
#-------------------------------------------------
include(../qumbia-plugins.pri)
include(../../cumbia-qtcontrols/cumbia-qtcontrols.pri)
include(../../qumbia-tango-controls/qumbia-tango-controls.pri)

QT       += core gui

TARGET = actions-extension-plugin
TEMPLATE = lib
CONFIG += plugin debug

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
