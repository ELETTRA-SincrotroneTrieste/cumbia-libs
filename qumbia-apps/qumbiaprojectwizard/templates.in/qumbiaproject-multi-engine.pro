isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT = PIPPO
}

include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

# CONFIG += debug | release

DEFINES += QT_NO_DEBUG_OUTPUT

# RESOURCES +=

SOURCES += src/main.cpp \
                src/$CPPFILE$

HEADERS += src/$HFILE$

# cuuimake runs uic
FORMS    = src/$FORMFILE$

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/$PROJECT_NAME$

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
    message("       execute `make install` to install '$PROJECT_NAME$' under $${INSTALL_DIR} ")
    message("-")
} else {
    message("-")
    message("INSTALLATION")
    message("       call qmake INSTALL_DIR=/usr/local/bin to install $PROJECT_NAME$ later with `make install` ")
    message("-")
}

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src
