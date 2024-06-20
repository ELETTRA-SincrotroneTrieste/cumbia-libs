isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}
include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

CONFIG += debug

DEFINES -= QT_NO_DEBUG_OUTPUT

# RESOURCES +=

TARGET       = bin/quplotrefreshtest

HEADERS = \
    circularbuffer.h \
    mainwindow.h \
    quplot.h \
    quplotpanel.h \
    quplotsettings.h

SOURCES = \
    circularbuffer.cpp \
    main.cpp \
    mainwindow.cpp \
    quplot.cpp \
    quplotpanel.cpp

target.path=$CUMBIA_ROOT/bin

INSTALLS += target
