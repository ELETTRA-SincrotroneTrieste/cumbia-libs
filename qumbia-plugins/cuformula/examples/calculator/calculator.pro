# include cumbia-qtcontrols for plugin path
include (/usr/local/cumbia-libs/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

include (/usr/local/cumbia-libs/include/qumbia-epics-controls/qumbia-epics-controls.pri)
include (/usr/local/cumbia-libs/include/qumbia-tango-controls/qumbia-tango-controls.pri)

TEMPLATE = app

QT +=  core

CONFIG += debug

CONFIG+=link_pkgconfig

DEFINES -= QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/calculator.cpp

HEADERS += src/calculator.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

INCLUDEPATH += /usr/local/cumbia-libs/include/qumbia-plugins/

TARGET   = bin/calculator

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

