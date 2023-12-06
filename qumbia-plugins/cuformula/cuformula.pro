include(../qumbia-plugins.pri)

include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

QT       += core gui script

DEFINES -= QT_NO_DEBUG_OUTPUT

TARGET = cuformula-plugin
TEMPLATE = lib
CONFIG += plugin release

# CONFIG += silent

SOURCES += \
    src/cuformulareader.cpp \
    src/cuformulaplugin.cpp \
    src/cuformula.cpp \
    src/cuformulaparser.cpp \
    src/cuformulautils.cpp \
    src/simpleformulaeval.cpp

HEADERS += \
    src/cuformulareader.h \
    src/cuformulaplugin.h \
    src/cuformula.h \
    src/cuformulaparser.h \
    src/cuformulautils.h \
    src/simpleformulaeval.h
DISTFILES += \ 
    cuformulasplugin.json

inc.files += $${HEADERS}

INCLUDEPATH += src

# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
