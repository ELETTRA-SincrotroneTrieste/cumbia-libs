#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T14:13:50
#
#-------------------------------------------------
include(../qumbia-plugins.pri)
include(../../cumbia-qtcontrols/cumbia-qtcontrols.pri)
include(../../qumbia-tango-controls/qumbia-tango-controls.pri)
include(../../qumbia-epics-controls/qumbia-epics-controls.pri)

QT       += core gui script

DEFINES -= QT_NO_DEBUG_OUTPUT

TARGET = cuformula-plugin
TEMPLATE = lib
CONFIG += plugin debug

# CONFIG += silent

SOURCES += \
    src/cuformulareader.cpp \
    src/cuformulaplugin.cpp \
    src/cuformula.cpp \
    src/cuformulaparser.cpp

HEADERS += \
    src/cuformulareader.h \
    src/cuformulaplugin.h \
    src/cuformula.h \
    src/cuformulaparser.h
DISTFILES += \ 
    cuformulasplugin.json

inc.files += $${HEADERS}

INCLUDEPATH += src

# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
