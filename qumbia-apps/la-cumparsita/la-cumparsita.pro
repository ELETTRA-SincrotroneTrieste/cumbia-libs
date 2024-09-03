isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

exists ($${INSTALL_ROOT}/include/quapps/quapps.pri) {
    include ($${INSTALL_ROOT}/include/quapps/quapps.pri)
} else {
    error("file $${INSTALL_ROOT}/include/quapps/quapps.pri not found")
}

TEMPLATE = app

QT +=  core gui uitools

DOCDIR = $${INSTALL_ROOT}/share/doc/la-cumparsita

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

# qt debug output
DEFINES += QT_NO_DEBUG_OUTPUT

isEmpty(buildtype) {
        buildtype = release
} else {
    equals(buildtype, debug) {
        message("")
        message("debug build")
        message("")
    }
}

CONFIG += $${buildtype}

OBJECTS_DIR = objects

# RESOURCES +=

SOURCES += src/main.cpp \
                src/cumparsita.cpp

HEADERS += src/cumparsita.h

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
