

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += CUMBIA_PRINTINFO

CONFIG += debug

SOURCES += main.cpp \
    propertyreader.cpp

INCLUDEPATH += /usr/local/include/cumbia /usr/local/include/cumbia-tango

LIBS += -L/usr/local/lib -lcumbia-tango -lcumbia

HEADERS += \
    propertyreader.h
