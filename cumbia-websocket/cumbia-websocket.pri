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

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local
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

!android-g++ {
    CONFIG += link_pkgconfig

    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia
    PKGCONFIG += cumbia-qtcontrols$${QTVER_SUFFIX}
    PKGCONFIG += cumbia-websocket$${QTVER_SUFFIX}
}

android-g++ {
    INCLUDEPATH += /usr/local/include/cumbia-qtcontrols
}

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x000001
VERSION = 0.0.1

DEFINES += CUMBIA_WEBSOCKET_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    CUMBIA_WEBSOCKET_VERSION=$${VERSION_HEX}


QMAKE_CXXFLAGS += -std=c++11 -Wall

CONFIG += c++11

MOC_DIR = moc
OBJECTS_DIR = obj

QMAKE_CLEAN = moc \
    obj \
    Makefile \
    *.tag

QMAKE_EXTRA_TARGETS += docs

SHAREDIR = $${INSTALL_ROOT}/share

doc.commands = doxygen \
    Doxyfile;

unix:android-g++ {
    unix:INCLUDEPATH += /usr/local/include/cumbia  /usr/local/include/cumbia-websocket
    unix:LIBS +=  -L/libs/armeabi-v7a/ -lcumbia
    unix:LIBS += -lcumbia-websocket$${QTVER_SUFFIX}
}

unix:!android-g++ {

    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia
    PKGCONFIG += cumbia-qtcontrols$${QTVER_SUFFIX}
    PKGCONFIG += cumbia-websocket$${QTVER_SUFFIX}

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





