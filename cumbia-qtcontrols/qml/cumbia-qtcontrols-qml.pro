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

#silent compiler
CONFIG += silent

TARGET = $$qtLibraryTarget($$TARGET)

uri = eu.elettra.cumbiaqmlcontrols

CONFIG += debug

QT +=  qml quick charts
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

DEFINES += CUMBIAQTCONTROLS_QML_HAS_CHARTS=1

SOURCES += \
    cumbiainitoptions.cpp \
    qmlreaderbackend.cpp \
    cumbiaqmlcontrolsplugin.cpp \
    qmlwriterbackend.cpp \
    qmlchartbackend.cpp

HEADERS += \
        qml_global.h \
    cumbiainitoptions.h \
    qmlreaderbackend.h \
    cumbiaqmlcontrolsplugin.h \
    qmlwriterbackend.h \
    qmlchartbackend.h 

DISTFILES += \
    cumbia-qtcontrols-qml.pri \
    qmldir \
    plugins.qmltypes \
    import/CumbiaCircularGauge.qml \
    import/CumbiaLabel.qml \
    jslib/CmdLineUtils.js \
    README.txt \
    import/CumbiaDial.qml \
    import/CuButton.qml \
    import/CuTumbler.qml \
    import/CuWButton.qml \
    import/CumbiaTrendChart.qml \
    import/CumbiaSpectrumChart.qml

designer.files =  \
    designer/cumbiaqmlcontrols.metainfo \
    designer/CumbiaCircularGaugeSpecifics.qml \
    designer/CumbiaLabelSpecifics.qml \
    designer/CumbiaTrendChartSpecifics.qml \
    designer/CumbiaSpectrumChartSpecifics.qml \
    designer/CumbiaDialSpecifics.qml \
    designer/CuTumblerSpecifics.qml

designer_imgs.files =  \
    designer/images/circulargauge-icon.png \
    designer/images/circulargauge-icon16.png \
    designer/images/lineseries-chart-icon.png \
    designer/images/lineseries-chart-icon16.png \
    designer/images/label-icon.png \
    designer/images/label-icon16.png \
    designer/images/dial-icon.png \
    designer/images/dial-icon16.png \
    designer/images/tumbler-icon.png \
    designer/images/tumbler-icon16.png \
    designer/images/button-icon16.png \
    designer/images/button-icon.png

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix:!android-g++ {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

    qmldir.path = $$installPath
    target.path = $$installPath

    designer.path = $$installPath/designer
    designer_imgs.path = $$installPath/designer/images
    other_files.path =  $$installPath
    other_files.files = $${DISTFILES}

    INSTALLS += target qmldir other_files designer designer_imgs


}

###
unix:android-g++ {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    pluginso.path = $$installPath
    pluginso.files = target
    pluginso.commands = cp lib$${TARGET}.so $$installPath

    message("android-g++: \"pluginso\" target is necessary to copy $${TARGET}.so into $$installPath")
    message($${TARGET}.so)
    message($$installPath)
    INSTALLS += pluginso
    qmldir.path = $$installPath
    target.path = $$installPath

    designer.path = $$installPath/designer
    designer_imgs.path = $$installPath/designer/images
    other_files.path =  $$installPath
    other_files.files = $${DISTFILES}

    INSTALLS += target qmldir other_files designer designer_imgs
}

unix {

    doc.commands = \
    doxygen \
    Doxyfile;

    doc.files = doc/*
    doc.path = $${CUMBIA_QTCONTROLS_QML_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc
}

