include(../../../qumbia-tango-controls/qumbia-tango-controls.pri)

QT += widgets

SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

TEMPLATE = app
CONFIG += debug 

SOURCES += \
           src/main.cpp \ 
    src/cuwatcher.cpp

HEADERS += \ 
    src/cuwatcher.h

FORMS = \
    src/cuwatcher.ui

TARGET = bin/cuwatcher




