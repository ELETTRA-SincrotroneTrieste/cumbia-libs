#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T10:23:51
#
#-------------------------------------------------
include(cumbia-random.pri)


QT       -= gui

CONFIG += debug

TARGET = cumbia-random$${QTVER_SUFFIX}
TEMPLATE = lib

DEFINES += CUMBIARANDOM_LIBRARY

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
        cumbiarandom.cpp \
    curndreader.cpp \
    cumbiarndworld.cpp \
    curndactionfactoryservice.cpp \
    curndactioni.cpp \
    curndactionreader.cpp \
    rnd_source.cpp \
    curandomgenactivity.cpp \
    curndactionfactories.cpp

HEADERS += \
        cumbiarandom.h \
        cumbia-random_global.h \ 
    curndreader.h \
    cumbiarndworld.h \
    curndactionfactoryservice.h \
    curndactioni.h \
    curndactionfactoryi.h \
    curndactionreader.h \
    rnd_source.h \
    curandomgenactivity.h \
    curndactionfactories.h

DISTFILES += \
    cumbia-random.pri


!android-g++ {
    PKGCONFIG -= cumbia-random$${QTVER_SUFFIX}
}

unix {

    doc.commands = \
    doxygen \
    Doxyfile;

    doc.files = doc/*
    doc.path = $${CUMBIA_RND_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}
    inc.path = $${CUMBIA_RND_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${CUMBIA_RND_INCLUDES}

    target.path = $${CUMBIA_RND_LIBDIR}
    INSTALLS += target inc other_inst

    !android-g++ {
            INSTALLS += doc
    }

# generate pkg config file
    CONFIG += create_pc create_prl no_install_prl

    QMAKE_PKGCONFIG_NAME = cumbia-random
    QMAKE_PKGCONFIG_DESCRIPTION = Qt random module for cumbia
    QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
    QMAKE_PKGCONFIG_LIBDIR = $${target.path}
    QMAKE_PKGCONFIG_INCDIR = $${inc.path}
    QMAKE_PKGCONFIG_VERSION = $${VERSION}
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
}

unix:INCLUDEPATH -= /usr/local/cumbia-libs/include/cumbia/cumbia-random

LIBS -= -lcumbia-random$${QTVER_SUFFIX}
