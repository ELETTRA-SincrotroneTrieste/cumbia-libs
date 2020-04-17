#-------------------------------------------------
#
# Project created by QtCreator 2017-04-14T11:52:03
#
#-------------------------------------------------
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

!wasm-emscripten {
    exists($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
        message("including support for qumbia-epics-controls module under $${INSTALL_ROOT}")
        include($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
    }

    exists($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
        message("including support for qumbia-tango-controls module under $${INSTALL_ROOT}")
        include($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
    }
} else {
    exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri) {
        message("including support for cumbia-qtcontrols module under $${INSTALL_ROOT}")
        include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
    }
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("including support for cumbia-random module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri) {
    message("including support for cumbia-websocket module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri)
}


QT       += core gui

CONFIG += debug

# silent compilation
# CONFIG += silent


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/qumbia-client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_NO_DEBUG_OUTPUT


SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/qumbia-client
DEFINES += DOC_PATH=\"\\\"$${DOCDIR}\\\"\"


CUMBIA_CLIENT_VERSION_HEX = 0x010000
CUMBIA_CLIENT_VERSION = 1.0.0

DEFINES += CUMBIA_CLIENT_VERSION_STR=\"\\\"$${CUMBIA_CLIENT_VERSION}\"\\\"


SOURCES += main.cpp \
    qumbia-client.cpp \
    writer.cpp

HEADERS  += \
    qumbia-client.h \
    writer.h

FORMS    += qumbia-client.ui

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin

INSTALLS += inst doc

