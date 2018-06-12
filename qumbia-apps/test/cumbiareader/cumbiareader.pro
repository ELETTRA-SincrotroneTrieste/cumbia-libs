include (/usr/local/include/qumbia-epics-controls/qumbia-epics-controls.pri)
include (/usr/local/include/qumbia-tango-controls/qumbia-tango-controls.pri)

TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

CONFIG +=

CONFIG+=link_pkgconfig
PKGCONFIG += x11

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/cumbiareader.cpp \
    src/reader.cpp

HEADERS += src/cumbiareader.h \
    src/reader.h

# cuuimake runs uic
# FORMS    = src/cumbiareader.ui

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/cumr_mul_noptr

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src


