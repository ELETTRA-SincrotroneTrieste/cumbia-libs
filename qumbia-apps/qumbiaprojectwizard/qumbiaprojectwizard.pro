#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T14:42:39
#
#-------------------------------------------------


# The application will be installed under INSTALL_ROOT (i.e. prefix)
#
# To set the prefix at build time, call 
# qmake   "INSTALL_ROOT=/my/custom/path"
#
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}


lessThan(QT_MAJOR_VERSION, 5) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}

CONFIG += debug

INCLUDEDIR = $${INSTALL_ROOT}/include

SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/qumbiaprojectwizard
DEFINES += DOC_PATH=\"\\\"$${DOCDIR}\\\"\"

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qumbiaprojectwizard

TEMPLATE = app

TEMPLATES_INSTALLDIR = $${SHAREDIR}/qumbiaprojectwizard

DEFINES += TEMPLATES_PATH=\"\\\"$${TEMPLATES_INSTALLDIR}\\\"\"

DEFINES += INCLUDE_PATH=\"\\\"$${INCLUDEDIR}\\\"\"

DEFINES -= QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        qumbiaprojectwizard.cpp \
    src/qtango/qtangoimport.cpp \
    src/qtango/main2cu.cpp \
    src/qtango/findreplace.cpp \
    src/qtango/definitions.cpp \
    src/qtango/pro_files_helper.cpp \
    src/qtango/cumbiacodeinjectcmd.cpp \
    src/qtango/codeextractors.cpp \
    src/qtango/codeinjector.cpp \
    src/qtango/maincpppreparecmd.cpp \
    src/qtango/conversiondialog.cpp \
    src/qtango/proconvertcmd.cpp \
    projectbackup.cpp \
    src/qtango/prosectionextractor.cpp \
    src/qtango/mainwidgetcppconstructprocesscmd.cpp \
    src/qtango/mainwidgethprocesscmd.cpp \
    src/qtango/maincppexpandcmd.cpp \
    src/qtango/cppinstantiationexpand.cpp

HEADERS  += qumbiaprojectwizard.h \
    src/qtango/qtangoimport.h \
    src/qtango/main2cu.h \
    src/qtango/findreplace.h \
    src/qtango/conversionhealth.h \
    src/qtango/fileprocessor_a.h \
    src/qtango/definitions.h \
    src/qtango/pro_files_helper.h \
    src/qtango/cumbiacodeinjectcmd.h \
    src/qtango/codeextractors.h \
    src/qtango/codeinjector.h \
    src/qtango/maincpppreparecmd.h \
    src/qtango/conversiondialog.h \
    src/qtango/proconvertcmd.h \
    projectbackup.h \
    src/qtango/prosectionextractor.h \
    src/qtango/mainwidgetcppconstructprocesscmd.h \
    src/qtango/mainwidgethprocesscmd.h \
    src/qtango/maincppexpandcmd.h \
    src/qtango/cppinstantiationexpand.h

FORMS    += qumbiaprojectwizard.ui

target.path = $${INSTALL_ROOT}/bin
target.files = $${TARGET}

templates.path = $${TEMPLATES_INSTALLDIR}
templates.files =  qumbia_project_template/*

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

INSTALLS = target templates doc

DISTFILES += \
    qumbia_project_template/qtango.keywords.json \
    qumbia_project_template/cppinstantiationexp.json

SUBDIRS += \
    qumbia_project_template/qumbiaproject-websocket.pro \
    qumbia_project_template/qumbiaprojectqml-tango-epics.pro

RESOURCES += \
    qumbia_project_template/qml.qrc
