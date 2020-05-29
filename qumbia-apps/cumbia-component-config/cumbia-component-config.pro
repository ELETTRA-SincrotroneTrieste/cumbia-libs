isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}
include($${INSTALL_ROOT}/include/quapps/quapps.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += debug

TARGET = bin/cu-component-config

SOURCES += \
        cu-module-component.cpp \
        history.cpp \
        main.cpp

HEADERS += \
    configcomponent_a.h \
    cu-module-component.h \
    history.h \
    term.h


doc.commands = \
doxygen \
Doxyfile;

DOCDIR = $${INSTALL_ROOT}/share/doc/cumbia-component-config
doc.files = doc/*
doc.path = $${DOCDIR}

QMAKE_EXTRA_TARGETS += doc

target.path = $${INSTALL_ROOT}/bin
INSTALLS += target doc
