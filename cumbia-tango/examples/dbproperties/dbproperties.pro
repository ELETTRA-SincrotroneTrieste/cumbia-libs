

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += CUMBIA_PRINTINFO

CONFIG += debug

SOURCES += main.cpp \
    propertyreader.cpp

INCLUDEPATH += /usr/local/cumbia-libs/include/cumbia /usr/local/cumbia-libs/include/cumbia-tango

LIBS += -L/usr/local/cumbia-libs/lib -lcumbia-tango -lcumbia

HEADERS += \
    propertyreader.h
