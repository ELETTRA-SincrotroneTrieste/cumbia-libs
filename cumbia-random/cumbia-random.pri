greaterThan(QT_MAJOR_VERSION, 4) {
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
    INSTALL_ROOT = /usr/local
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

    packagesExist(qwt){
        PKGCONFIG += qwt
        QWT_PKGCONFIG = qwt
        message("Qwt: using pkg-config to configure qwt includes and libraries")
    }
    else:packagesExist(Qt5Qwt6){
        PKGCONFIG += Qt5Qwt6
        QWT_PKGCONFIG = Qt5Qwt6
        message("Qwt: using pkg-config to configure qwt includes and libraries (Qt5Qwt6)")
    } else {
        warning("Qwt: no pkg-config file found")
        warning("Qwt: export PKG_CONFIG_PATH=/usr/path/to/qwt/lib/pkgconfig if you want to enable pkg-config for qwt")
        warning("Qwt: if you build and install qwt from sources, be sure to uncomment/enable ")
        warning("Qwt: QWT_CONFIG     += QwtPkgConfig in qwtconfig.pri qwt project configuration file")
    }
}

android-g++ {
    INCLUDEPATH += /usr/local/include/cumbia-qtcontrols
}

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x000001
VERSION = 0.0.1

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
    unix:INCLUDEPATH += /usr/local/include/cumbia
    unix:LIBS +=  -L/libs/armeabi-v7a/ -lcumbia
    unix:LIBS += -lcumbia-random
}

unix:!android-g++ {

    isEmpty(QWT_PKGCONFIG){
        message("no Qwt pkg-config file found")
        message("adding $${QWT_INCLUDES} and $${QWT_INCLUDES_USR} to include path")
        message("adding  -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QTVER_SUFFIX} to libs")
        message("this should work for ubuntu installations")

        unix:INCLUDEPATH += /usr/local/include/cumbia
        unix:LIBS += -L/usr/local/lib -lcumbia
    }

    LIBS += -lcumbia-random

}





