isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri):exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri) {
  include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
  include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
} else {
    error("randomwi.pro: this example requires the cumbia-random and cumbia-qtcontrols modules installed under $${INSTALL_ROOT}")
}

TEMPLATE = app

QT +=  core gui

CONFIG +=

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/randomwi.cpp

HEADERS += src/randomwi.h

# cuuimake runs uic
# FORMS    = src/randomwi.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
HEADERS += \
    ui/ui_randomwi.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/randomwi

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

QMAKE_CXXFLAGS += -s \"BINARYEN_TRAP_MODE=\'clamp\'\"

QMAKE_LFLAGS +=  -s \"BINARYEN_TRAP_MODE=\'clamp\'\"


message("-")
message("NOTE")
message("You need to run cuuimake in order to build the project")
message("")
message("        cuuimake --show-config to see cuuimake configuration options")
message("        cuuimake --configure to configure cuuimake")
message("        cuuimake -jN to execute cuuimake and then make -jN")
message("        cuuimake --make to run cuuimake and then make")
message("-")
