isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}
include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

CONFIG += debug


CONFIG+=link_pkgconfig
PKGCONFIG += x11

DEFINES -= QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
    src/context.cpp

HEADERS += \
    src/context.h

# cuuimake runs uic
# FORMS    = src/options.ui

# where to find cuuimake ui_*.h files
# since FORMS is not used
#
INCLUDEPATH += ui

TARGET   = bin/context

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

message("")
message("NOTE")
message("You need to run cuuimake in order to build the project")
message("")
message("        cuuimake --show-config to see cuuimake configuration options")
message("        cuuimake --configure to configure cuuimake")
message("        cuuimake -jN to execute cuuimake and then make -jN")
message("        cuuimake --make to run cuuimake and then make")
message("")
