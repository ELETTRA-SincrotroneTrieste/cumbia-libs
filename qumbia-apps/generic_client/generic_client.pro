#-------------------------------------------------
#
# Project created by QtCreator 2017-04-14T11:52:03
#
#-------------------------------------------------

include(../../qumbia-epics-controls/qumbia-epics-controls.pri)
include(../../qumbia-tango-controls/qumbia-tango-controls.pri)

QT       += core gui

CONFIG += debug


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = generic_client
TEMPLATE = app

CONFIG+=link_pkgconfig
PKGCONFIG += epics-base

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES -= QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp \
    generic_client.cpp \
    src/element.cpp

HEADERS  += \
    generic_client.h \
    src/element.h

FORMS    += generic_client.ui

