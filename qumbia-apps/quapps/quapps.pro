isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

# INSTALL_ROOT is used to install the target
# prefix is used within DEFINES +=
#
# cumbia installation script uses a temporary INSTALL_ROOT during build
# and then files are copied into the destination prefix. That's where
# configuration files must be found by the application when the script
# installs everything at destination
#
isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}

linux-g++ {
    exists ($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
        message("- adding EPICS support under $${INSTALL_ROOT}")
        include ($${INSTALL_ROOT}/include/qumbia-epics-controls/qumbia-epics-controls.pri)
    }
    exists  ($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
        message("- adding Tango support under $${INSTALL_ROOT}")
        include ($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
    }
    greaterThan(QT_MAJOR_VERSION, 4): QT += 
} else {
    # include cumbia-qtcontrols for necessary qt engine-unaware dependency (widgets, qwt, ...)
    include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("- adding cumbia-random module support under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri) {
    message("- adding cumbia-websocket module support under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-http/cumbia-http.pri) {
    message("including support for cumbia-http module under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-http/cumbia-http.pri)
}

DOCDIR=$${INSTALL_ROOT}/share/doc/quapps

HEADERS += \
    quapps.h

DISTFILES += \
    quapps.md \
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
inst.path = $${INSTALL_ROOT}/include/quapps
target.path = /tmp

INSTALLS += inst doc target



