isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
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
    greaterThan(QT_MAJOR_VERSION, 4): QT += x11extras
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

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# silent compilation
# CONFIG += silent


TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_NO_DEBUG_OUTPUT

unix:INCLUDEPATH += $${INSTALL_ROOT}/include/quapps

