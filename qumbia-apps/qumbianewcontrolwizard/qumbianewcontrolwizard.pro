#-------------------------------------------------
#
# Project created by QtCreator 2018-04-10T15:14:45
#
#-------------------------------------------------

QT       += core gui

INSTALL_ROOT = /usr/local

greaterThan(QT_MAJOR_VERSION, 4) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}

CONFIG += debug

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share

DOCDIR = $${INSTALL_ROOT}/share/doc/qumbiacontrolwizard

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATES_INSTALLDIR = $${SHAREDIR}/qumbiacontrolwizard

QUMBIANEWCONTROLSWIZARD_VERSION_HEX = 0x010000
QUMBIANEWCONTROLSWIZARD_VERSION = 1.0.0

DEFINES += QUMBIANEWCONTROLSWIZARD_VERSION_STR=\"\\\"$${QUMBIANEWCONTROLSWIZARD_VERSION}\"\\\"

DEFINES += TEMPLATES_PATH=\"\\\"$${TEMPLATES_INSTALLDIR}\\\"\"

DEFINES += INCLUDE_PATH=\"\\\"$${INCLUDEDIR}/cumbia-qtcontrols\\\"\"

DEFINES += DOC_PATH=\"\\\"$${SHAREDIR}/doc\\\"\"

DEFINES -= QT_NO_DEBUG_OUTPUT


TARGET = qumbianewcontrolwizard
TEMPLATE = app

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
        src/main.cpp \
        src/qumbianewcontrolw.cpp

HEADERS += \
        src/qumbianewcontrolw.h

FORMS += \
        src/qumbianewcontrolw.ui

target.path = $${INSTALL_ROOT}/bin
target.files = $${TARGET}

templates.path = $${TEMPLATES_INSTALLDIR}
templates.files =  qumbiacontrolwizard_template/*

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}

INSTALLS = target templates doc

DISTFILES += \
    qumbiacontrolwizard_template/header_r.h \
    qumbiacontrolwizard_template/header_w.h \
    qumbiacontrolwizard_template/header_rw.h \
    qumbiacontrolwizard_template/cpp_r.cpp \
    qumbiacontrolwizard_template/cpp_w.cpp \
    qumbiacontrolwizard_template/cpp_rw.cpp
