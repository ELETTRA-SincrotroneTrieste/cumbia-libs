isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

exists ($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
    message("- adding EPICS support under $${INSTALL_ROOT}")
    include ($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
}
exists  ($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
    message("- adding Tango support under $${INSTALL_ROOT}")
    include ($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("- adding cumbia-random module support under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}

TEMPLATE = app

CONFIG +=  console
QT -= gui

SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/qumbia-reader
EXAMPLESDIR = $${SHAREDIR}/examples/qumbia-reader
DEFINES += DOC_PATH=\"\\\"$${DOCDIR}\\\"\"

CONFIG +=

# CONFIG+=qml_debug
# CONFIG+=declarative_debug

CONFIG+=link_pkgconfig
PKGCONFIG -= x11

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

SOURCES += src/main.cpp \
    src/cmdlineoptions.cpp \
    src/kbdinputwaitthread.cpp \
    src/qumbia-reader.cpp \
    src/qureader.cpp \
    src/rconfig.cpp \
    src/tgdbprophelper.cpp

HEADERS += \
    src/cmdlineoptions.h \
    src/kbdinputwaitthread.h \
    src/qumbia-reader.h \
    src/qureader.h \
    src/rconfig.h \
    src/tgdbprophelper.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/qumbia-reader
doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

QMAKE_STRIP=echo

inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin
inst.extra = strip $(TARGET); cp -f $(TARGET) $${INSTALL_ROOT}/bin

examples.files = js_functions/sin.js
examples.path = $${EXAMPLESDIR}/js_functions

script.path = $${INSTALL_ROOT}/bin
script.files = qutangoreader/qutangoreader
# INSTALLS tries to strip the bash script


INSTALLS += inst doc script examples

DISTFILES += \
    js_functions/sin.js \
    qutangoreader/qutangoreader

RESOURCES += \
    qumbia-reader.qrc

