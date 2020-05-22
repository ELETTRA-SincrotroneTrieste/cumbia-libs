isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}

linux-g++ {
    exists ($${CUMBIA_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
        message("- adding EPICS support under $${CUMBIA_ROOT}")
        include ($${CUMBIA_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
    }
    exists  ($${CUMBIA_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
        message("- adding Tango support under $${CUMBIA_ROOT}")
        include ($${CUMBIA_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
    }
    greaterThan(QT_MAJOR_VERSION, 4): QT += x11extras
} else {
    # include cumbia-qtcontrols for necessary qt engine-unaware dependency (widgets, qwt, ...)
    include ($${CUMBIA_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
}

exists($${CUMBIA_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("- adding cumbia-random module support under $${CUMBIA_ROOT}")
    include($${CUMBIA_ROOT}/include/cumbia-random/cumbia-random.pri)
}

exists($${CUMBIA_ROOT}/include/cumbia-websocket/cumbia-websocket.pri) {
    message("- adding cumbia-websocket module support under $${CUMBIA_ROOT}")
    include($${CUMBIA_ROOT}/include/cumbia-websocket/cumbia-websocket.pri)
}

exists($${CUMBIA_ROOT}/include/cumbia-http/cumbia-http.pri) {
    message("including support for cumbia-http module under $${CUMBIA_ROOT}")
    include($${CUMBIA_ROOT}/include/cumbia-http/cumbia-http.pri)
}

DOCDIR=$${CUMBIA_ROOT}/share/doc/quapps

HEADERS += \
    quapps.h

DISTFILES += \
    quapps.pri

## compile a dummy main.cpp
##
SOURCES += dummy.cpp
TARGET = bin/dummy

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

inst.files = $${HEADERS} $${DISTFILES}
inst.path = $${CUMBIA_ROOT}/include/quapps

INSTALLS += inst doc



