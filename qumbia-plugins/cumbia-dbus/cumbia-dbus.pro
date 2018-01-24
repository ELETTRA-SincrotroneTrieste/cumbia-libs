# $Id: dbus.pro,v 1.9 2014-08-28 13:23:45 giacomo Exp $
# $Name $


include(/usr/local/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

TEMPLATE = lib
CONFIG += plugin

QT += dbus xml

INCLUDEPATH = src $${INCLUDEPATH}

DBUS_ADAPTORS += src/cumbiadbus.xml
DBUS_INTERFACES += src/cumbiadbus.xml

INC_DIR = $${CUMBIA_QTCONTROLS_INCLUDES}/plugins
SHAREDIR = $${CUMBIA_QTCONTROLS_SHARE}/plugins
PLUGIN_LIB_DIR = $${CUMBIA_QTCONTROLS_LIBDIR}/cumbia-qtcontrols/plugins
DESTDIR = plugins

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

DOC_DIR = $${SHAREDIR}
doc.commands = \
    doxygen \
    Doxyfile;
doc.files = doc/
doc.path = $${DOC_DIR}

# lib

target.path = $${PLUGIN_LIB_DIR}

inc.files += $${HEADERS} cumbiadbus_interface.h

inc.path = $${INC_DIR}

# installation

INSTALLS += target \
    inc \
    doc

DISTFILES += \
    quapplication.xml
