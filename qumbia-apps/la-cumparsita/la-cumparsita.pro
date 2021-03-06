
# INSTALL_ROOT is used to install the target
# prefix is used within DEFINES +=
#
# cumbia installation script uses a temporary INSTALL_ROOT during build
# and then files are copied into the destination prefix. That's where
# configuration files must be found by the application when the script
# installs everything at destination
#
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}
exists($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
    message("including support for qumbia-epics-controls module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
}

exists($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
    message("including support for qumbia-tango-controls module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("including support for cumbia-random module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}

include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

TEMPLATE = app

QT +=  core gui uitools

DOCDIR = $${INSTALL_ROOT}/share/doc/la-cumparsita

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras


DEFINES += CUMBIA_QTCONTROLS_PLUGIN_DIR=\"\\\"$${prefix}/lib/qumbia-plugins\\\"\"
# qt debug output
DEFINES += QT_NO_DEBUG_OUTPUT

CONFIG+=link_pkgconfig
PKGCONFIG += x11

OBJECTS_DIR = objects

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
    jscripts/spectrum_diff.js \
    jscripts/spectrum_avg.js

target.path = $${INSTALL_ROOT}/bin

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}

QMAKE_EXTRA_TARGETS += doc

INSTALLS += target doc
