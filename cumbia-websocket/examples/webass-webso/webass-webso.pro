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


TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG +=

DEFINES -= QT_NO_DEBUG_OUTPUT

# RESOURCES +=


SOURCES += src/main.cpp \
                src/webass-webso.cpp

HEADERS += src/webass-webso.h

# cuuimake runs uic
# FORMS    = src/webass-webso.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
HEADERS += \
    ui/ui_webass-webso.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

wasm-emscripten{
    TARGET=wasm/webass-webso
}else{
    TARGET=bin/webass-webso
}

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

#
# make install works if INSTALL_DIR is given to qmake
#
!isEmpty(INSTALL_DIR) {
    wasm-emscripten {
        inst.files = wasm/*
    } else {
        inst.files = $${TARGET}
    }
    inst.path = $${INSTALL_DIR}
    INSTALLS += inst
    message("-")
    message("INSTALLATION")
    message("       execute `make install` to install 'webass-webso' under $${INSTALL_DIR} ")
    message("-")
} else {
    message("-")
    message("INSTALLATION")
    message("       call qmake INSTALL_DIR=/usr/local/bin to install webass-webso later with `make install` ")
    message("-")
}


# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

message("-")
message("NOTE")
message("You need to run cuuimake in order to build the project")
message("-")
message("        cuuimake --show-config to see cuuimake configuration options")
message("        cuuimake --configure to configure cuuimake")
message("        cuuimake -jN to execute cuuimake and then make -jN")
message("        cuuimake --make to run cuuimake and then make")
message("-")
