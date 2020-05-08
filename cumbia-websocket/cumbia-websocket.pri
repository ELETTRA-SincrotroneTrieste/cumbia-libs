QT += websockets

lessThan(QT_MAJOR_VERSION, 5) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}

# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here qumbia-websocket will be installed
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
    CUMBIA_WS_INCLUDES=$${INSTALL_ROOT}/include/cumbia-websocket
#
#
# Here qumbia-tango-controls share files will be installed
#
    CUMBIA_WS_SHARE=$${INSTALL_ROOT}/share/cumbia-websocket
##
#
# Here qumbia-tango-controls libraries will be installed
    CUMBIA_WS_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    CUMBIA_WS_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-websocket
#
# The name of the library
    cumbia_ws_LIB=cumbia-websocket$${QTVER_SUFFIX}
#
# Here qumbia-tango-controls libraries will be installed
    CUMBIA_WS_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    CUMBIA_QTCONTROLS_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-websocket
#

android-g++|wasm-emscripten {
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia
    PKGCONFIG += cumbia-qtcontrols$${QTVER_SUFFIX}
    PKGCONFIG += cumbia-websocket$${QTVER_SUFFIX}
}

android-g++ {
    INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-qtcontrols
    OBJECTS_DIR = obj-android
}
wasm-emscripten {
    INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia-qtcontrols $${INSTALL_ROOT}/include/cumbia $${CUMBIA_WS_INCLUDES}
    OBJECTS_DIR = obj-wasm
    QMAKE_WASM_PTHREAD_POOL_SIZE=16
}

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x010102
VERSION = 1.1.2

DEFINES += CUMBIA_WEBSOCKET_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    CUMBIA_WEBSOCKET_VERSION=$${VERSION_HEX}


QMAKE_CXXFLAGS += -std=c++11 -Wall

CONFIG += c++11

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
    unix:INCLUDEPATH += $${INSTALL_ROOT}/include/cumbia  $${INSTALL_ROOT}/include/cumbia-websocket
    unix:LIBS +=  -L/libs/armeabi-v7a/ -lcumbia
    unix:LIBS += -lcumbia-websocket$${QTVER_SUFFIX}
}

wasm-emscripten {
    CONFIG += link_prl
}

android-g++|wasm-emscripten {
    LIBS += -L$${INSTALL_ROOT}/lib/wasm -lcumbia-websocket$${QTVER_SUFFIX} -lcumbia
} else {
    OBJECTS_DIR = objs
    packagesExist(cumbia):packagesExist(cumbia-qtcontrols$${QTVER_SUFFIX}) {
        message("cumbia-websocket.pri: using pkg-config to configure cumbia cumbia-qtcontrols$${QTVER_SUFFIX} includes and libraries")
    } else {
        packagesExist(cumbia):packagesExist(cumbia-qtcontrols$${QTVER_SUFFIX}):packagesExist(cumbia-websocket$${QTVER_SUFFIX}) {
            message("cumbia-websocket.pri: using pkg-config to configure cumbia cumbia-qtcontrols$${QTVER_SUFFIX} and cumbia-websocket$${QTVER_SUFFIX} includes and libraries")
        } else {
            unix:INCLUDEPATH += /usr/local/include/cumbia  /usr/local/include/cumbia-websocket
            unix:LIBS +=  -L/usr/local/lib -lcumbia
            unix:LIBS += -lcumbia-websocket$${QTVER_SUFFIX}
        }
    }
}





