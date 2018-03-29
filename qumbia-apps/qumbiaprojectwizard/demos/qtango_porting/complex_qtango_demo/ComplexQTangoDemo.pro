include (/usr/local/qtango/include/qtango6/qtango.pri)

TEMPLATE = app

QT += 

CONFIG +=

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES += 

UI_DIR = ui

INCLUDEPATH += src

SOURCES += src/main.cpp \
		src/ComplexQTangoDemo.cpp \
    src/tdialread.cpp \
    src/tdialwrite.cpp \
    src/demodialog.cpp

HEADERS += src/ComplexQTangoDemo.h \
    src/tdialread.h \
    src/tdialwrite.h \
    src/demodialog.h

FORMS    = src/ComplexQTangoDemo.ui \
    src/demodialog.ui

TARGET   = bin/complex_qtango_demo

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src 

