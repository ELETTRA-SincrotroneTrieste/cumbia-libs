
# install root
#
exists(../cumbia-qt.prf) {
    include(../cumbia-qt.prf)
}


wasm-emscripten {
# library is compiled statically
# cumbia-random needs Qt Script
    QT += script
    OBJECTS_DIR = obj-wasm
    QMAKE_WASM_PTHREAD_POOL_SIZE=16
}

# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here qumbia-random will be installed
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
# Here qumbia-rnd include files will be installed
    CUMBIA_RND_INCLUDES=$${INSTALL_ROOT}/include/cumbia-random
#
#
# Here cumbia-random share files will be installed
#
    CUMBIA_RND_SHARE=$${INSTALL_ROOT}/share/cumbia-random
##
#
# Here cumbia-random libraries will be installed
    CUMBIA_RND_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here cumbia-random documentation will be installed
    CUMBIA_RND_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-random

#
# The name of the library
    cumbia_rnd_LIB=cumbia-random
#
# Here qumbia-random libraries will be installed
    wasm-emscripten {
        CUMBIA_RND_LIBDIR=$${INSTALL_ROOT}/lib/wasm
    } else {
        CUMBIA_RND_LIBDIR=$${INSTALL_ROOT}/lib
    }
#
#
# Here cumbia-random documentation will be installed
    CUMBIA_QTCONTROLS_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-random
#

android-g++|wasm-emscripten {
} else {

    OBJECTS_DIR = objects

    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia
    PKGCONFIG += cumbia-qtcontrols
    PKGCONFIG += cumbia-random

    packagesExist(cumbia):packagesExist(cumbia-qtcontrols) {
        message("Qwt: using pkg-config to configure cumbia cumbia-qtcontrols includes and libraries")
    } else {
        packagesExist(cumbia):packagesExist(cumbia-qtcontrols):packagesExist(cumbia-random) {
            message("Qwt: using pkg-config to configure cumbia cumbia-qtcontrols includes and libraries")
        }
    }
}

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x020100
VERSION = 2.1.0

DEFINES += CUMBIA_RANDOM_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    CUMBIA_RANDOM_VERSION=$${VERSION_HEX}


QMAKE_CXXFLAGS += -std=gnu++17 -Wall

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
    unix:INCLUDEPATH += /usr/local/include/cumbia /usr/local/include/cumbia/cumbia-random
    unix:LIBS +=  -L/libs/armeabi-v7a/ -lcumbia
    unix:LIBS += -lcumbia-random
} else {
    wasm-emscripten {
        LIBS +=  -L$${CUMBIA_RND_LIBDIR}/wasm -lcumbia-random
        INCLUDEPATH += $${CUMBIA_RND_INCLUDES} $${INSTALL_ROOT}/include/cumbia-qtcontrols $${INSTALL_ROOT}/include/cumbia

    } else {
        packagesExist(cumbia):packagesExist(cumbia-qtcontrols) {
        } else {
            message("either cumbia or cumbia-qtcontrols pkg-config files found")
            unix:INCLUDEPATH += /usr/local/include/cumbia /usr/local/include/cumbia/cumbia-random
            unix:LIBS += -L/usr/local/lib -lcumbia
            LIBS += -lcumbia-random
        }
    }
}
