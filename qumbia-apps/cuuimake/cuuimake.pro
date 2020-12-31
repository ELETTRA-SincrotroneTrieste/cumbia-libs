# The application will be installed under INSTALL_ROOT (i.e. prefix)
#
# To set the prefix at build time, call 
# qmake   "INSTALL_ROOT=/my/custom/path"
#
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

lessThan(QT_MAJOR_VERSION, 5) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}

# INSTALL_ROOT is used to install the target
# prefix is used within DEFINES +=
#
# cumbia installation script uses a temporary INSTALL_ROOT during build
# and then files are copied into the destination prefix. That's where
# configuration files must be found by the application when the script
# installs everything at destination
#
isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}

SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/cuuimake

DEFINES_SHAREDIR = $${prefix}/share
DEFINES_DOCDIR = $${DEFINES_SHAREDIR}/doc/cuuimake
#
# where config files are found by the application at runtime: use prefix
#
DEFINES += CONFDIR=\"\\\"$${DEFINES_SHAREDIR}/cuuimake\"\\\"
DEFINES += CUUIMAKE_DOCDIR=\"\\\"$${DEFINES_DOCDIR}\"\\\"

# version
CUUIMAKE_VERSION_HEX = 0x010000
CUUIMAKE_VERSION = 1.0.0
DEFINES += CUUIMAKE_VERSION_STR=\"\\\"$${CUUIMAKE_VERSION}\"\\\"


QT -= gui

QT += xml

CONFIG += c++11 console

CONFIG += debug

CONFIG -= app_bundle

CONFIG += silent

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_NO_DEBUG_OUTPUT



# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    src/defs.h \
    src/cuuimake.h \
    src/conf.h \
    src/parser.h \
    src/processor.h \
    src/uicproc.h \
    src/options.h \
    src/xmakeprocess.h

SOURCES += src/main.cpp \
    src/defs.cpp \
    src/cuuimake.cpp \
    src/conf.cpp \
    src/parser.cpp \
    src/processor.cpp \
    src/uicproc.cpp \
    src/options.cpp \
    src/xmakeprocess.cpp

DISTFILES += \
    cuuimake-cumbia-qtcontrols.xml

TARGET = bin/cuuimake

target.path = $${INSTALL_ROOT}/bin

conf.path = $${INSTALL_ROOT}/share/cuuimake
conf.files = cuuimake-cumbia-qtcontrols.xml

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}

QMAKE_EXTRA_TARGETS += doc

INSTALLS += target conf doc

