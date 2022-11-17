isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}
include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

# CONFIG += debug | release

DEFINES += QT_NO_DEBUG_OUTPUT

# RESOURCES +=

SOURCES += src/main.cpp \
                src/engine-switch.cpp

HEADERS += src/engine-switch.h

# cuuimake runs uic
# FORMS    = src/engine-switch.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
HEADERS += \
    ui/ui_engine-switch.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET = engine-switch

!wasm-emscripten {
    TARGET   = bin/$${TARGET}
} else {
    TARGET = wasm/$${TARGET}
}

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
    message("       execute `make install` to install 'engine-switch' under $${INSTALL_DIR} ")
    message("-")
} else {
    message("-")
    message("INSTALLATION")
    message("       call qmake INSTALL_DIR=/usr/local/bin to install engine-switch later with `make install` ")
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
