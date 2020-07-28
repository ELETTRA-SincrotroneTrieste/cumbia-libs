include(qumbia-epics-controls.pri)

QT       += widgets

# Debug or release?
CONFIG += debug

# silent compiler
CONFIG += silent

PKGCONFIG -= qumbia-epics-controls$${QTVER_SUFFIX}

# defined in qumbia-epics-controls.pri
TARGET = $${QUMBIA_EPICS_CONTROLS_LIB}

TEMPLATE = lib

DEFINES += QUMBIAEPICSCONTROLS_LIBRARY CUMBIA_DEBUG_OUTPUT

DEFINES -= QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/cuepregisterengine.cpp \
    src/qumbiaepicscontrols.cpp \
    src/cuepcontrolswriter.cpp \
    src/cuepcontrolsreader.cpp \
    src/cuepcontrols-utils.cpp \
    src/quepicsplotupdatestrategy.cpp

HEADERS +=\
    src/cuepcontrols-utils.h \
    src/cuepcontrolsreader.h \
    src/cuepcontrolswriter.h \
    src/cuepregisterengine.h \
    src/qumbia-epics-controls_global.h \
    src/qumbiaepicscontrols.h \
    src/quepicsplotupdatestrategy.h

DISTFILES += \
    qumbia-epics-controls.pri \
    README

unix {
    doc.commands = doxytag \
    -t \
    qt.tag \
    /usr/share/qt4/doc/html/noplease; \
    doxygen \
    Doxyfile;
    doc.files = doc/*
    doc.path = $${QUMBIA_EPICS_CONTROLS_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}

    inc.path = $${QUMBIA_EPICS_CONTROLS_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${QUMBIA_EPICS_CONTROLS_INCLUDES}

    target.path = $${QUMBIA_EPICS_CONTROLS_LIBDIR}
    INSTALLS += target doc inc other_inst

# generate pkg config file
    CONFIG += create_pc create_prl no_install_prl

    QMAKE_PKGCONFIG_NAME = qumbia-epics-controls
    QMAKE_PKGCONFIG_DESCRIPTION = Qt controls widget for graphical interfaces over cumbia and the Tango control system
    QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
    QMAKE_PKGCONFIG_LIBDIR = $${target.path}
    QMAKE_PKGCONFIG_INCDIR = $${inc.path}
    QMAKE_PKGCONFIG_VERSION = $${VERSION}
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
}

packagesExist(cumbia-epics) {

!isEmpty(EPICS_BASE) {
        unix:INCLUDEPATH += $${EPICS_BASE}/include $${EPICS_BASE}/include/os/Linux  $${EPICS_BASE}/include/compiler/gcc \
            $${EPICS_BASE}/include/os/compiler/clang
        unix:LIBS+=-L$${EPICS_BASE}/lib/$${EPICS_HOST_ARCH}
}


# remove ourselves (added in .pri)
#
unix:INCLUDEPATH -=  $${QUMBIA_EPICS_CONTROLS_INCLUDES}

unix:LIBS -=  \
    -L$${QUMBIA_EPICS_CONTROLS_LIBDIR} -l$${QUMBIA_EPICS_CONTROLS_LIB}

FORMS += \
    src/rtconfwidget.ui
