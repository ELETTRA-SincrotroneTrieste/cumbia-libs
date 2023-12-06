include (/usr/local/qtango/include/qtango6/qtango.pri)

TEMPLATE = app

QT +=  core


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

CONFIG +=

CONFIG+=link_pkgconfig
PKGCONFIG += x11

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/qtangoreader.cpp \
    src/reader.cpp

HEADERS += src/qtangoreader.h \
    src/reader.h

INCLUDEPATH += src

TARGET   = bin/qtangoreader

