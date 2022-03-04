isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}


# include cumbia-qtcontrols for necessary qt engine-unaware dependency (widgets, qwt, ...)
include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)


exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("- adding cumbia-random module support under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}




TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG +=

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/cu_qtcontrols_widgets.cpp

HEADERS += src/cu_qtcontrols_widgets.h

# cuuimake runs uic
# FORMS    = src/cu_qtcontrols_widgets.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
HEADERS += \
    ui/ui_cu_qtcontrols_widgets.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

wasm-emscripten {
    TARGET   = wasm/cu_qtcontrols_widgets
} else {
    TARGET = bin/cu_qtcontrols_widgets
}

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

!isEmpty(INSTALL_DIR) {
    wasm-emscripten {
        inst.files = wasm/cu_qtcontrols_widgets/*
    } else {
        inst.files = $${TARGET}
    }
    inst.path = $${INSTALL_DIR}
    INSTALLS += inst
    message("-")
    message("INSTALLATION")
    message("       execute `make install` to install 'cu_qtcontrols_widgets' under $${INSTALL_DIR} ")
    message("-")
} else {
    message("-")
    message("INSTALLATION")
    message("       call qmake INSTALL_DIR=/usr/local/bin to install cu_qtcontrols_widgets later with `make install` ")
    message("-")
}

LIBS += -L$${INSTALL_ROOT}/lib/wasm -lcumbia

QMAKE_WASM_PTHREAD_POOL_SIZE=12

# unix:INCLUDEPATH +=  . ../../src
