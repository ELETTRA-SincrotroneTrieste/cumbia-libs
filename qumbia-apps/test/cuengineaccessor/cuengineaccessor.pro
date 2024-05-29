isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}
include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

# CONFIG += debug | release

DEFINES += QT_NO_DEBUG_OUTPUT

# RESOURCES +=

SOURCES += src/main.cpp \
    src/cuengineaccessor_test.cpp

HEADERS += \
    src/cuengineaccessor_test.h

# cuuimake runs uic
FORMS    = \
    src/cuengineaccessor_test.ui

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET = cuengineaccessor

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
    message("       execute `make install` to install 'cuengineaccessor' under $${INSTALL_DIR} ")
    message("-")
} else {
    message("-")
    message("INSTALLATION")
    message("       call qmake INSTALL_DIR=/usr/local/bin to install cuengineaccessor later with `make install` ")
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
