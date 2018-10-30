#-------------------------------------------------
#
# Project created by QtCreator 2018-10-30T11:01:05
#
#-------------------------------------------------
include(../qumbia-plugins.pri)
include(../../qumbia-tango-controls/qumbia-tango-controls.pri)

QT       += core gui

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
