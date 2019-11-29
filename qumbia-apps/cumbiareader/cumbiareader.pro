exists (/usr/local/cumbia-libs/include/qumbia-epics-controls/qumbia-epics-controls.pri) {
    message("- adding EPICS support")
    include (/usr/local/cumbia-libs/include/qumbia-epics-controls/qumbia-epics-controls.pri)
}
exists  (/usr/local/cumbia-libs/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
    message("- adding Tango support")
    include (/usr/local/cumbia-libs/include/qumbia-tango-controls/qumbia-tango-controls.pri)
}

exists(/usr/local/cumbia-libs/include/cumbia-random/cumbia-random.pri) {
    message("- adding cumbia-random module support")
    include(/usr/local/cumbia-libs/include/cumbia-random/cumbia-random.pri)
}

TEMPLATE = app

QT +=  core gui


SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/cumbiareader
DEFINES += DOC_PATH=\"\\\"$${DOCDIR}\\\"\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

CONFIG +=

# CONFIG+=qml_debug
# CONFIG+=declarative_debug

CONFIG+=link_pkgconfig
PKGCONFIG += x11

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES +=


SOURCES += src/main.cpp \
    src/cmdlineoptions.cpp \
                src/cumbiareader.cpp \
    src/rconfig.cpp \
    src/reader.cpp \
    src/tgdbprophelper.cpp

HEADERS += src/cumbiareader.h \
    src/cmdlineoptions.h \
    src/rconfig.h \
    src/reader.h \
    src/tgdbprophelper.h

# cuuimake runs uic
# FORMS    = src/cumbiareader.ui

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET   = bin/cumbiareader

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

BASH_COMPLETION_DIR=$$system(pkg-config --variable=completionsdir bash-completion)
completion.path = $${BASH_COMPLETION_DIR}
completion.files = bash_completion.d/cumbiareader

message(" ")
message("bash completion dir is $${BASH_COMPLETION_DIR}")
message(" ")

QMAKE_STRIP=echo

inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin
inst.extra = strip $(TARGET); cp -f $(TARGET) $${INSTALL_ROOT}/bin

script.path = $${INSTALL_ROOT}/bin
script.files = qutangoreader/qutangoreader
# INSTALLS tries to strip the bash script


INSTALLS += inst doc completion script

DISTFILES += \
    bash_completion.d/cumbiareader \
    js_functions/sin.js \
    qutangoreader/qutangoreader

