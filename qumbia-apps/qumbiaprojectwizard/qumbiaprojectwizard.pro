#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T14:42:39
#
#-------------------------------------------------
INSTALL_ROOT = /usr/local

greaterThan(QT_MAJOR_VERSION, 4) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qumbiaprojectwizard

TEMPLATE = app

TEMPLATES_INSTALLDIR = $${SHAREDIR}/qumbiaprojectwizard

DEFINES += TEMPLATES_PATH=\"\\\"$${TEMPLATES_INSTALLDIR}\\\"\"

DEFINES += INCLUDE_PATH=\"\\\"$${INCLUDEDIR}\\\"\"

DEFINES -= QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        qumbiaprojectwizard.cpp

HEADERS  += qumbiaprojectwizard.h

FORMS    += qumbiaprojectwizard.ui

target.path = $${INSTALL_ROOT}/bin
target.files = $${TARGET}

templates.path = $${TEMPLATES_INSTALLDIR}
templates.files =  qumbia_project_template/*

INSTALLS = target templates

DISTFILES +=
