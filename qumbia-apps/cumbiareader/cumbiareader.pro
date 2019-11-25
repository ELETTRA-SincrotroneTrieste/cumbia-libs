include (/usr/local/cumbia-libs/include/qumbia-epics-controls/qumbia-epics-controls.pri)
include (/usr/local/cumbia-libs/include/qumbia-tango-controls/qumbia-tango-controls.pri)

exists(/usr/local/cumbia-libs/include/cumbia-random/cumbia-random.pri) {
    message("including support for cumbia-random module")
    include(/usr/local/cumbia-libs/include/cumbia-random/cumbia-random.pri)
}

TEMPLATE = app

QT +=  core gui


SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/cumbiareader
DEFINES += DOC_PATH=\"\\\"$${DOCDIR}\\\"\"

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

TARGET   = bin/cumbiareader

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin

INSTALLS += inst doc

