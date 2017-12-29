#-------------------------------------------------
#
# Project created by QtCreator 2017-12-27T15:40:52
#
#-------------------------------------------------



include(/usr/local/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

QT       += core gui

TARGET = cumbia-multiread
TEMPLATE = lib
CONFIG += plugin debug

INC_DIR = $${CUMBIA_QTCONTROLS_INCLUDES}/plugins
SHAREDIR = $${CUMBIA_QTCONTROLS_SHARE}/plugins
PLUGIN_LIB_DIR = $${CUMBIA_QTCONTROLS_LIBDIR}/cumbia-qtcontrols/plugins

DESTDIR = plugins

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    qumultireader.cpp

HEADERS += \
    qumultireader.h
DISTFILES += cumbia-multiread.json 

DOC_DIR = $${SHAREDIR}
doc.commands = \
    doxygen \
    Doxyfile;
doc.files = doc/
doc.path = $${DOC_DIR}

# lib

target.path = $${PLUGIN_LIB_DIR}

inc.files += $${HEADERS}

inc.path = $${INC_DIR}

# installation

INSTALLS += target \
    inc \
    doc

