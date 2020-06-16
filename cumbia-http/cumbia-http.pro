#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T10:23:51
#
#-------------------------------------------------
include(cumbia-http.pri)


isEmpty(CU_USER_CONFIG_DIR) {
    CU_USER_CONFIG_DIR=.config/cumbia
}

DEFINES += CUMBIA_USER_CONFIG_DIR=\"\\\"$${CU_USER_CONFIG_DIR}\\\"\"

#
# CuHttpAuthManager checks if qApp is an instance of QApplication and
# can pop up a message box for authentication if so.
#
isEmpty(QT_NO_WIDGETS) {
    message("-")
    message("cumbia-http.pro: QT widgets enabled for authorization manager dialog")
    message("cumbia-http.pro: You can disable QT widgets by calling qmake QT_NO_WIDGETS=1")
    message("-")
    QT       += widgets
}

CONFIG += debug

# CONFIG += silent

isEmpty(DEFAULT_CHANNEL_MSG_TTL) {
    DEFAULT_CHANNEL_MSG_TTL=5
    message("cumbia-http.pro: setting default channel message ttl to $${DEFAULT_CHANNEL_MSG_TTL}")
    message("cumbia-http.pro: change it executing qmake DEFAULT_CHANNEL_MSG_TTL=X")
}

PKGCONFIG -= cumbia-http$${QTVER_SUFFIX}

TARGET = cumbia-http$${QTVER_SUFFIX}
TEMPLATE = lib

DEFINES += CUMBIAHTTP_LIBRARY
DEFINES += CUMBIA_DEBUG_OUTPUT=1 -DDEFAULT_CHAN_MSG_TTL=$${DEFAULT_CHANNEL_MSG_TTL}

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
    cuhttp_source.cpp \
    cuhttpactiona.cpp \
    cuhttpactionconf.cpp \
    cuhttpactionfactories.cpp \
    cuhttpactionfactoryservice.cpp \
    cuhttpactionreader.cpp \
    cuhttpactionwriter.cpp \
    cuhttpauthcookiestore.cpp \
    cuhttpauthmanager.cpp \
    cuhttpauthmanagertools.cpp \
    cuhttpchannelreceiver.cpp \
    cuhttpcontrolsreader.cpp \
    cuhttpcontrolswriter.cpp \
    cuhttpregisterengine.cpp \
    cumbiahttp.cpp \
    cumbiahttpworld.cpp \
    protocol/tango/cuhttpprotocolhelpers.cpp \
    protocol/tango/cuhttptangohelper.cpp \
    protocol/tango/cuhttptangoreplacewildcards.cpp \
    protocol/tango/cuhttptangosrc.cpp \
    protocol/tango/cuhttptangosrchelper.cpp

HEADERS += \
    cuhttp_source.h \
    cuhttpactiona.h \
    cuhttpactionconf.h \
    cuhttpactionfactories.h \
    cuhttpactionfactoryi.h \
    cuhttpactionfactoryservice.h \
    cuhttpactionreader.h \
    cuhttpactionwriter.h \
    cuhttpauthcookiestore.h \
    cuhttpauthmanager.h \
    cuhttpauthmanagertools.h \
    cuhttpchannelreceiver.h \
    cuhttpcontrolsreader.h \
    cuhttpcontrolswriter.h \
    cuhttpregisterengine.h \
    cuhttpsrchelper_i.h \
    cumbiahttp.h \
    cumbiahttpworld.h \
    protocol/cuhttpprotocolhelper_i.h \
    protocol/tango/cuhttpprotocolhelpers.h \
    protocol/tango/cuhttptangohelper.h \
    protocol/tango/cuhttptangoreplacewildcards.h \
    protocol/tango/cuhttptangosrc.h \
    protocol/tango/cuhttptangosrchelper.h

INCLUDEPATH += protocol protocol/tango

DISTFILES += \
    cumbia-http.pri \
    android/README.openssl.txt

unix {

    doc.commands = \
    doxygen \
    Doxyfile;

    doc.files = doc/*
    doc.path = $${CUMBIA_HTTP_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}
    inc.path = $${CUMBIA_HTTP_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${CUMBIA_HTTP_INCLUDES}

android-g++|wasm-emscripten {
    target.path = $${CUMBIA_HTTP_LIBDIR}/wasm
} else {
    target.path = $${CUMBIA_HTTP_LIBDIR}
}
    INSTALLS += target inc other_inst

    !android-g++ {
            INSTALLS += doc
    }

android-g++|wasm-emscripten {
} else {
    # generate pkg config file
        CONFIG += create_pc create_prl no_install_prl

        QMAKE_PKGCONFIG_NAME = cumbia-http
        QMAKE_PKGCONFIG_DESCRIPTION = Qt http module for cumbia
        QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
        QMAKE_PKGCONFIG_LIBDIR = $${target.path}
        QMAKE_PKGCONFIG_INCDIR = $${inc.path}
        QMAKE_PKGCONFIG_VERSION = $${VERSION}
        QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    }
}

wasm-emscripten {
CONFIG += create_prl
}

LIBS -= -lcumbia-http$${QTVER_SUFFIX}

LIBS += -lcumbia-qtcontrols$${QTVER_SUFFIX}
