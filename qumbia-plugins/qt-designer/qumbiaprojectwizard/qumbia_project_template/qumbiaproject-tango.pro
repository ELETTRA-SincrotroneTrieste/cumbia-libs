include ($INCLUDE_DIR$/qumbia-tango-controls/qumbia-tango-controls.pri)

TEMPLATE = app

QT +=  core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

CONFIG +=


CONFIG+=link_pkgconfig
PKGCONFIG += x11

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

