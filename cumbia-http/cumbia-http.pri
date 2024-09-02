
# install root
#
exists(../cumbia-qt.prf) {
    include(../cumbia-qt.prf)
}


QT += network websockets

# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here qumbia-http will be installed
# INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
#
# WebAssembly: includes under $${INSTALL_ROOT}/include/cumbia-random, libs under $${INSTALL_ROOT}/lib/wasm
# cumbia and cumbia-qtcontrols includes searched under $${INSTALL_ROOT}/include/cumbia and
# $${INSTALL_ROOT}/include/cumbia-qtcontrols
#
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}
#
#
# Here qumbia-ws include files will be installed
    CUMBIA_HTTP_INCLUDES=$${INSTALL_ROOT}/include/cumbia-http
#
#
# Here qumbia-tango-controls share files will be installed
#
    CUMBIA_HTTP_SHARE=$${INSTALL_ROOT}/share/cumbia-http
##
#
# Here qumbia-tango-controls libraries will be installed
    CUMBIA_HTTP_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    CUMBIA_HTTP_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-http
#
# The name of the library
    cumbia_http_LIB=cumbia-http
#
# Here qumbia-tango-controls libraries will be installed
    CUMBIA_HTTP_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    CUMBIA_QTCONTROLS_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-http
#

android-g++ {
    INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-qtcontrols
    OBJECTS_DIR = obj-android
}
wasm-emscripten {
    INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-qtcontrols $${INSTALL_ROOT}/include/cumbia $${CUMBIA_HTTP_INCLUDES}
    OBJECTS_DIR = obj-wasm
    QMAKE_WASM_PTHREAD_POOL_SIZE=16
}

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x020115
VERSION = 2.1.15

DEFINES += CUMBIA_HTTP_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    CUMBIA_HTTP_VERSION=$${VERSION_HEX}

MOC_DIR = moc

QMAKE_CLEAN = moc \
    objects \
    Makefile \
    *.tag

QMAKE_EXTRA_TARGETS += docs

SHAREDIR = $${INSTALL_ROOT}/share

doc.commands = doxygen \
    Doxyfile;

unix:android-g++ {
    unix:INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia  $${INSTALL_ROOT}/include/cumbia-http
    unix:LIBS +=  -L/libs/armeabi-v7a/ -lcumbia
    unix:LIBS += -lcumbia-http
}

wasm-emscripten {
    CONFIG += link_prl
}

android-g++|wasm-emscripten {
    LIBS += -L$${INSTALL_ROOT}/lib/wasm -lcumbia-http -lcumbia
} else {
    OBJECTS_DIR = objects

    exists($${INSTALL_ROOT}/include/cumbia) {
        INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia
        LIBS += -L$${INSTALL_ROOT}/lib -lcumbia
    } else {
        error("cumbia installation not found under $${INSTALL_ROOT}")
    }
    # libs at this point contain -L$${INSTALL_ROOT}/lib

    exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri) {
        INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-qtcontrols
        LIBS += -lcumbia-qtcontrols
    } else {
        error("cumbia-qtcontrols installation not found under $${INSTALL_ROOT}")
    }

    exists($${INSTALL_ROOT}/include/cumbia-http) {
        INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-http
        LIBS += -l$${cumbia_http_LIB}
    }  # else: do not output an error here: lib build would fail
}
