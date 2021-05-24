
include(/usr/local/cumbia-libs/include/quapps/quapps.pri)

TEMPLATE = app

QT += core gui widgets

CONFIG += debug

DEFINES -= QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES += 


SOURCES += src/main.cpp \
		src/create-delete.cpp

HEADERS += src/create-delete.h

FORMS    =

TARGET   = bin/create-delete

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src 

DISTFILES += \
    README.txt

