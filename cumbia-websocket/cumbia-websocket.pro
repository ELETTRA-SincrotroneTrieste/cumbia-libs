#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T10:23:51
#
#-------------------------------------------------
include(cumbia-websocket.pri)


QT       -= gui

QT += websockets

CONFIG += debug

# CONFIG += silent

PKGCONFIG -= cumbia-websocket$${QTVER_SUFFIX}

TARGET = cumbia-websocket$${QTVER_SUFFIX}
TEMPLATE = lib

DEFINES += CUMBIAWEBSOCKET_LIBRARY
DEFINES += CUMBIA_DEBUG_OUTPUT=1

DEFINES -= QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cumbiawebsocket.cpp \
    cuwscontrolsreader.cpp \
    cuwscontrolswriter.cpp \
    cumbiawsworld.cpp \
    cuwsactionfactoryservice.cpp \
    cuwsactionreader.cpp \
    cuwsactionwriter.cpp \
    cuwsactionconf.cpp \
    protocol/tango/cuwsprotocolhelpers.cpp \
    protocol/tango/cuwstangohelper.cpp \
    protocol/tango/cuwstangoreplacewildcards.cpp \
    ws_source.cpp \
    cuwsactionfactories.cpp \
    cuwsclient.cpp

HEADERS += \
        cumbiawebsocket.h \
        cumbia-websocket_global.h \
    cuwscontrolsreader.h \
    cuwscontrolswriter.h \
    cumbiawsworld.h \
    cuwsactionfactoryservice.h \
    cuwsactioni.h \
    cuwsactionfactoryi.h \
    cuwsactionreader.h \
    cuwsactionconf.h \
    cuwsactionwriter.h \
    protocol/cuwsprotocolhelper_i.h \
    protocol/tango/cuwsprotocolhelpers.h \
    protocol/tango/cuwstangohelper.h \
    protocol/tango/cuwstangoreplacewildcards.h \
    ws_source.h \
    cuwsactionfactories.h \
    cuwsclient.h

INCLUDEPATH += protocol protocol/tango

DISTFILES += \
    cumbia-websocket.pri \
    android/README.openssl.txt

unix {

    doc.commands = \
    doxygen \
    Doxyfile;

    doc.files = doc/*
    doc.path = $${CUMBIA_WS_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}
    inc.path = $${CUMBIA_WS_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${CUMBIA_WS_INCLUDES}

android-g++|wasm-emscripten {
    target.path = $${CUMBIA_WS_LIBDIR}/wasm
} else {
    target.path = $${CUMBIA_WS_LIBDIR}
}
    INSTALLS += target inc other_inst

    !android-g++ {
            INSTALLS += doc
    }

android-g++|wasm-emscripten {
} else {
    # generate pkg config file
        CONFIG += create_pc create_prl no_install_prl

        QMAKE_PKGCONFIG_NAME = cumbia-websocket
        QMAKE_PKGCONFIG_DESCRIPTION = Qt websocket module for cumbia
        QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
        QMAKE_PKGCONFIG_LIBDIR = $${target.path}
        QMAKE_PKGCONFIG_INCDIR = $${inc.path}
        QMAKE_PKGCONFIG_VERSION = $${VERSION}
        QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    }
}

LIBS -= -lcumbia-websocket$${QTVER_SUFFIX}
