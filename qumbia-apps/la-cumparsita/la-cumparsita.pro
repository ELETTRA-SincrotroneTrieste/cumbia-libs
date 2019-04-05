include (/usr/local/cumbia-libs/include/qumbia-epics-controls/qumbia-epics-controls.pri)
include (/usr/local/cumbia-libs/include/qumbia-tango-controls/qumbia-tango-controls.pri)
include (/usr/local/cumbia-libs/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

TEMPLATE = app

QT +=  core gui uitools

DOCDIR = $${INSTALL_ROOT}/share/doc/la-cumparsita

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

CONFIG +=

CONFIG+=link_pkgconfig
PKGCONFIG += x11

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += CUMBIA_QTCONTROLS_PLUGIN_DIR=\"\\\"$${INSTALL_ROOT}/lib/qumbia-plugins\\\"\"

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/cumparsita.cpp

HEADERS += src/cumparsita.h

# cuuimake runs uic
# FORMS    = src/cumparsita.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
# HEADERS += \
#    ui/ui_cumparsita.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += src

TARGET   = bin/la-cumparsita

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

DISTFILES += \
    jscripts/spectrum_sum.js \
    jscripts/spectrum_diff.js

target.path = $${INSTALL_ROOT}/bin

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}

QMAKE_EXTRA_TARGETS += doc

INSTALLS += target doc
