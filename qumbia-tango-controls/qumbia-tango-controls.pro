include(qumbia-tango-controls.pri)

QT       += widgets

# Debug or release?
CONFIG += release

# silent
CONFIG += silent

PKGCONFIG -= qumbia-tango-controls$${QTVER_SUFFIX}

# defined in qumbia-tango-controls.pri
TARGET = $${QUMBIA_TANGO_CONTROLS_LIB}

TEMPLATE = lib

DEFINES += QUMBIATANGOCONTROLS_LIBRARY=1 CUMBIA_PRINTINFO=1

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

SOURCES += src/qumbiatangocontrols.cpp \
    src/cutangoregisterengine.cpp \
    src/cutangoreplacewildcards.cpp \
    src/cutcontrolsreader.cpp \
    src/cutcontrols-utils.cpp \
    src/cutcontrolswriter.cpp \
    src/qutplotupdatestrategy.cpp \
    src/qutplotcontextmenustrategy.cpp \
    src/quconnectionsettingswidget.cpp \
    src/rtconfwidget.cpp \
    src/qutrealtimeplot.cpp

HEADERS += src/qumbiatangocontrols.h\
    src/cutangoregisterengine.h \
    src/cutangoreplacewildcards.h \
    src/plugin_ifaces/qutfindsrcsplugini.h \
        src/qumbia-tango-controls_global.h \
    src/cutcontrolsreader.h \
    src/cutcontrols-utils.h \
    src/cutcontrolswriter.h \
    src/qutplotupdatestrategy.h \
    src/qutplotcontextmenustrategy.h \
    src/quconnectionsettingswidget.h \
    src/rtconfwidget.h \
    src/qutrealtimeplot.h

DISTFILES += \
    qumbia-tango-controls.pri \
    README \
    src/tutorial_faq.md

unix {
    doc.commands = doxygen Doxyfile;
    doc.files = doc/*
    doc.path = $${QUMBIA_TANGO_CONTROLS_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}

    inc.path = $${QUMBIA_TANGO_CONTROLS_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${QUMBIA_TANGO_CONTROLS_INCLUDES}

    target.path = $${QUMBIA_TANGO_CONTROLS_LIBDIR}
    INSTALLS += target doc inc other_inst

# generate pkg config file
    CONFIG += create_pc create_prl no_install_prl

    QMAKE_PKGCONFIG_NAME = qumbia-tango-controls
    QMAKE_PKGCONFIG_DESCRIPTION = Qt controls widget for graphical interfaces over cumbia and the Tango control system
    QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
    QMAKE_PKGCONFIG_LIBDIR = $${target.path}
    QMAKE_PKGCONFIG_INCDIR = $${inc.path}
    QMAKE_PKGCONFIG_VERSION = $${VERSION}
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    QMAKE_PKGCONFIG_REQUIRES = cumbia-tango cumbia-qtcontrols
}

# remove ourselves (added in .pri)
#
unix:INCLUDEPATH -= \
    $${QUMBIA_TANGO_CONTROLS_INCLUDES}

unix:LIBS -=  \
    -L$${QUMBIA_TANGO_CONTROLS_LIBDIR} -l$${QUMBIA_TANGO_CONTROLS_LIB}

FORMS += \
    src/rtconfwidget.ui
