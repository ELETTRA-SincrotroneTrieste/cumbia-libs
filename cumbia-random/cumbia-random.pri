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
# Here qumbia-random will be installed
# INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
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
# Here qumbia-tango-controls share files will be installed
#
    CUMBIA_RND_SHARE=$${INSTALL_ROOT}/share/cumbia-random
##
#
# Here qumbia-tango-controls libraries will be installed
    CUMBIA_RND_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    CUMBIA_RND_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-random
#
# The name of the library
    cumbia_rnd_LIB=cumbia-random$${QTVER_SUFFIX}
#
# Here qumbia-tango-controls libraries will be installed
    CUMBIA_RND_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    CUMBIA_QTCONTROLS_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-random
#

!android-g++ {
    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia
    PKGCONFIG += cumbia-qtcontrols$${QTVER_SUFFIX}
    PKGCONFIG += cumbia-random$${QTVER_SUFFIX}

    packagesExist(cumbia):packagesExist(cumbia-qtcontrols$${QTVER_SUFFIX}) {
        message("Qwt: using pkg-config to configure cumbia cumbia-qtcontrols$${QTVER_SUFFIX} includes and libraries")
    } else {
        packagesExist(cumbia):packagesExist(cumbia-qtcontrols$${QTVER_SUFFIX}):packagesExist(cumbia-random$${QTVER_SUFFIX}) {
            message("Qwt: using pkg-config to configure cumbia cumbia-qtcontrols$${QTVER_SUFFIX} includes and libraries")
        }
    }
}

android-g++ {
    INCLUDEPATH += /usr/local/include/cumbia-qtcontrols
}

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x010100
VERSION = 1.1.0

DEFINES += CUMBIA_RANDOM_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    CUMBIA_RANDOM_VERSION=$${VERSION_HEX}


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
    unix:INCLUDEPATH += /usr/local/include/cumbia /usr/local/include/cumbia/cumbia-random
    unix:LIBS +=  -L/libs/armeabi-v7a/ -lcumbia
    unix:LIBS += -lcumbia-random$${QTVER_SUFFIX}
}

unix:!android-g++ {
    packagesExist(cumbia):packagesExist(cumbia-qtcontrols$${QTVER_SUFFIX}) {
    } else {
        message("no cumbia/cumbia-qtcontrols/cumbia-random pkg-config files found")
        unix:INCLUDEPATH += /usr/local/include/cumbia /usr/local/include/cumbia/cumbia-random
        unix:LIBS += -L/usr/local/lib -lcumbia
        LIBS += -lcumbia-random$${QTVER_SUFFIX}
    }



}

HEADERS += \
    $$PWD/curndfunctiongeni.h

SOURCES +=





