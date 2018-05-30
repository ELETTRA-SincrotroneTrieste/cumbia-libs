include (/usr/local/qtango/include/qtango6/qtango.pri)

TEMPLATE = app

QT += 

CONFIG +=

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES += 


SOURCES += src/main.cpp \
		src/bpmqtango.cpp

HEADERS += src/bpmqtango.h

FORMS    = src/bpmqtango.ui

TARGET   = bin/bpmqtango

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src 

