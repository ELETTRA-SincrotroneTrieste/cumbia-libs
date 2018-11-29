include(/usr/local/cumbia-libs/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

DEFINES -= QT_NO_DEBUG_OUTPUT

TEMPLATE = app
CONFIG += c++11
QT += core gui widgets

SOURCES += main.cpp \
    activity.cpp \
    simplea.cpp

HEADERS += \
    activity.h \
    simplea.h
