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
}

exists($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri) {
    message("- adding cumbia-random module support under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-random/cumbia-random.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri) {
    message("- adding cumbia-websocket module support under $${INSTALL_ROOT}")
    include($${INSTALL_ROOT}/include/cumbia-websocket/cumbia-websocket.pri)
}


TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG +=

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/$CPPFILE$

HEADERS += src/$HFILE$

# cuuimake runs uic
# FORMS    = src/$FORMFILE$
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
HEADERS += \
    ui/$UI_FORMFILE_H$

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/$PROJECT_NAME$

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
