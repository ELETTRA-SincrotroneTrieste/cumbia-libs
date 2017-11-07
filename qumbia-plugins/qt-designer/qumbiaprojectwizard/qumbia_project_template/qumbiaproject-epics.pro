include ($INCLUDE_DIR$/qumbia-epics-controls/qumbia-epics-controls.pri)

TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

CONFIG +=


CONFIG+=link_pkgconfig
PKGCONFIG += epics-base x11

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
                src/$CPPFILE$

HEADERS += src/$HFILE$

# cuuimake runs uic
# FORMS    = src/$FORMFILE$

# where to find cuuimake ui_*.h files
# since FORMS is not used
#
INCLUDEPATH += ui

TARGET   = bin/$PROJECT_NAME$

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

