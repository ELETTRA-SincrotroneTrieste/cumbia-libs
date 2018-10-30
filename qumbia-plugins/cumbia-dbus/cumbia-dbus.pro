# $Id: dbus.pro,v 1.9 2014-08-28 13:23:45 giacomo Exp $
# $Name $

include(../qumbia-plugins.pri)
include(../../cumbia-qtcontrols/cumbia-qtcontrols.pri)

TEMPLATE = lib
CONFIG += plugin

QT += dbus xml

INCLUDEPATH = src $${INCLUDEPATH}

DBUS_ADAPTORS += src/cumbiadbus.xml
DBUS_INTERFACES += src/cumbiadbus.xml

TARGET = $$qtLibraryTarget(cumbia-dbus-plugin)

DEFINES -= QT_NO_DEBUG_OUTPUT

# Input
HEADERS += \
    src/cumbiadbusplugin.h \
    src/quappdbus.h \
    src/quappdbuscontroller.h

SOURCES += \
    src/cumbiadbusplugin.cpp \
    src/quappdbus.cpp \
    src/quappdbuscontroller.cpp

inc.files += $${HEADERS} cumbiadbus_interface.h

# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.

DISTFILES += \
    quapplication.xml
