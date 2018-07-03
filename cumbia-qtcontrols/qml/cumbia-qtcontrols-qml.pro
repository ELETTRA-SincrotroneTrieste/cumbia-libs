#-------------------------------------------------
#
# Project created by QtCreator 2018-06-26T14:23:57
#
#-------------------------------------------------
include (../cumbia-qtcontrols.pri)

#
# The name of the library
    cumbia_qtcontrols_qml_LIB=cumbia-qtcontrols-qml$${QTVER_SUFFIX}plugin

CUMBIA_QTCONTROLS_QML_DOCDIR = $${CUMBIA_QTCONTROLS_DOCDIR}/qml

# remove ourselves (defined in .pri)
PKGCONFIG -= cumbia-qtcontrols$${QTVER_SUFFIX}

unix:INCLUDEPATH -= \
    $${QUMBIA_TANGO_CONTROLS_INCLUDES}

unix:LIBS -= \
    -l$${cumbia_qtcontrols_qml_LIB}

TARGET = $${cumbia_qtcontrols_qml_LIB}
TEMPLATE = lib
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)

uri = eu.elettra.cumbiaqmlcontrols

CONFIG += debug

QT +=  qml quick
QT       -= gui

DEFINES += QML_LIBRARY

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
    cumbiainitoptions.cpp \
    qmlreaderbackend.cpp \
    cumbiaqmlcontrolsplugin.cpp

HEADERS += \
        qml_global.h \
    cumbiainitoptions.h \
    qmlreaderbackend.h \
    cumbiaqmlcontrolsplugin.h

DISTFILES += \
    cumbia-qtcontrols-qml.pri \
    qmldir \
    import/MyRectangle.qml \
    plugins.qmltypes \
    import/CumbiaCircularGauge.qml \
    import/CumbiaLabel.qml

designer.files =  \
    designer/cumbiaqmlcontrols.metainfo \
    designer/CumbiaCircularGaugeSpecifics.qml \
    designer/CumbiaLabelSpecifics.qml

designer_imgs.files =  \
    designer/images/circulargauge-icon.png \
    designer/images/circulargauge-icon16.png


!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath

    designer.path = $$installPath/designer
    designer_imgs.path = $$installPath/designer/images
    other_files.path =  $$installPath
    other_files.files = $${DISTFILES}

    INSTALLS += target qmldir other_files designer designer_imgs
}


#unix {

#    doc.commands = \
#    doxygen \
#    Doxyfile;

#    doc.files = doc/*
#    doc.path = $${CUMBIA_QTCONTROLS_QML_DOCDIR}
#    QMAKE_EXTRA_TARGETS += doc

#    inc.files = $${HEADERS}
#    inc.path = $${CUMBIA_QTCONTROLS_INCLUDES}/qml

## cumbia-qtcontrols-qml.pri
#    other_inst.files = $${DISTFILES}
#    other_inst.path = $${CUMBIA_QTCONTROLS_INCLUDES}/qml

#    target.path = $${CUMBIA_QTCONTROLS_LIBDIR}

#    INSTALLS += target doc inc other_inst

## generate pkg config file
#    CONFIG += create_pc create_prl no_install_prl

#    QMAKE_PKGCONFIG_NAME = cumbia-qtcontrols
#    QMAKE_PKGCONFIG_DESCRIPTION = Qt controls widget for graphical interfaces over cumbia
#    QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
#    QMAKE_PKGCONFIG_LIBDIR = $${target.path}
#    QMAKE_PKGCONFIG_INCDIR = $${inc.path}
#    QMAKE_PKGCONFIG_VERSION = $${VERSION}
#    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
#}


