#-------------------------------------------------
#
# Project created by QtCreator 2017-12-27T15:40:52
#
#-------------------------------------------------
include(../qumbia-plugins.pri)
include(../../cumbia-qtcontrols/cumbia-qtcontrols.pri)

TARGET = cumbia-multiread-plugin
TEMPLATE = lib
CONFIG += plugin debug

SOURCES += \
    qumultireader.cpp

HEADERS += \
    qumultireader.h
DISTFILES += cumbia-multiread.json 

inc.files += $${HEADERS}


# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
