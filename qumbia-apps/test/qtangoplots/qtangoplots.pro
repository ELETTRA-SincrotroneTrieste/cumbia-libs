include (/usr/local/qtango/include/qtango6/qtango.pri)

TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

CONFIG +=


CONFIG+=link_pkgconfig
PKGCONFIG += x11

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/qtangoplots.cpp

HEADERS += src/qtangoplots.h

# cuuimake runs uic
FORMS    = src/qtangoplots.ui

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/qtangoplots

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

