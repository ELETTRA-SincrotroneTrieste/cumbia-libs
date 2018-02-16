TEMPLATE = app
CONFIG += c++11
QT += core gui widgets

unix:INCLUDEPATH += ../../src/lib ../../src/lib/threads

unix:LIBS += -L../../.libs  -lcumbia

SOURCES += main.cpp \
    activity.cpp \
    simplea.cpp

HEADERS += \
    activity.h \
    simplea.h
