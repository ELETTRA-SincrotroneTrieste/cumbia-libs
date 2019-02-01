include(/usr/local/cumbia-libs/include/qumbia-tango-controls/qumbia-tango-controls.pri)
include(/usr/local/cumbia-libs/include/qumbia-epics-controls/qumbia-epics-controls.pri)

QT -= gui

QT += network sql

QT -= QT_NO_DEBUG_OUTPUT

CONFIG+=link_pkgconfig

CONFIG += debug

TARGET = bin/cumbia-telegram

PKGCONFIG += tango
PKGCONFIG += x11

packagesExist(cumbia) {
    PKGCONFIG += cumbia
}

packagesExist(cumbia-tango) {
    PKGCONFIG += cumbia-tango
}

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


INCLUDEPATH += src

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/main.cpp \
    src/cubotserver.cpp \
    src/cubotlistener.cpp \
    src/botdb.cpp \
    src/tbotmsg.cpp \
    src/tbotmsgdecoder.cpp \ 
	src/botreader.cpp \
    src/cubotsender.cpp \
    src/cumbiasupervisor.cpp \
    src/msgformatter.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/cubotserver.h \
    src/cubotlistener.h \
    src/botdb.h \
    src/tbotmsg.h \
    src/tbotmsgdecoder.h \ 
	src/botreader.h \
    src/cubotsender.h \
    src/cumbiasupervisor.h \
    src/msgformatter.h
