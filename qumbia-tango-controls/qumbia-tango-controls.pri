#-------------------------------------------------
#
# Project created by QtCreator 2017-04-12T16:47:19
#
#-------------------------------------------------


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
# Here qumbia-tango-controls will be installed
    INSTALL_ROOT = /usr/local
#
#
# Here qumbia-tango-controls include files will be installed
    QUMBIA_TANGO_CONTROLS_INCLUDES=$${INSTALL_ROOT}/include/qumbia-tango-controls
#
#
# Here qumbia-tango-controls share files will be installed
#
    QUMBIA_TANGO_CONTROLS_SHAREDIR=$${INSTALL_ROOT}/share/qumbia-tango-controls
#
#
# Here qumbia-tango-controls libraries will be installed
    QUMBIA_TANGO_CONTROLS_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-tango-controls documentation will be installed
    QUMBIA_TANGO_CONTROLS_DOCDIR=$${INSTALL_ROOT}/share/doc/qumbia-tango-controls
#
# The name of the library
    QUMBIA_TANGO_CONTROLS_LIB=qumbia-tango-controls$${QTVER_SUFFIX}
#
#
#
# ======================== DEPENDENCIES =================================================
#
# Qwt libraries (>= 6.1.2) are installed here:
#   QWT_HOME =
#

exists(/usr/local/qwt-6.1.3) {
    QWT_HOME = /usr/local/qwt-6.1.3
}

QWT_LIB = qwt

QWT_INCLUDES=$${QWT_HOME}/include

QWT_HOME_USR = /usr
QWT_INCLUDES_USR = $${QWT_HOME_USR}/include/qwt

CONFIG+=link_pkgconfig
PKGCONFIG += tango cumbia cumbia-tango cumbia-qtcontrols$${QTVER_SUFFIX} qumbia-tango-controls$${QTVER_SUFFIX}

QWT_PKGCONFIG =

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

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x000001
VERSION = 0.0.1
VER_MAJ = 0
VER_MIN = 0
VER_FIX = 1

DEFINES += QUMBIA_TANGO_CONTROLS_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QUMBIA_TANGO_CONTROLS_VERSION=$${VERSION_HEX} \
    VER_MAJ=$${VER_MAJ} \
    VER_MIN=$${VER_MIN} \
    VER_FIX=$${VER_FIX}

DEFINES += CUMBIA_QTCONTROLS_PLUGIN_DIR=\"\\\"$${INSTALL_ROOT}/lib/cumbia-qtcontrols\\\"\"

QMAKE_CXXFLAGS += -std=c++11 -Wall


freebsd-g++ {
    message( )
    message( *)
    message( * Compiling under FreeBSD)
    message( * :-P)
    message( )
    unix:LIBS -= -ldl
    QMAKE_CXXFLAGS -= -std=c++0x
}

CONFIG += c++11

MOC_DIR = moc
OBJECTS_DIR = obj
FORMS_DIR = ui
LANGUAGE = C++
UI_DIR = src
QMAKE_DEL_FILE = rm \
    -rf
QMAKE_CLEAN = moc \
    obj \
    Makefile \
    *.tag

QMAKE_EXTRA_TARGETS += docs

SHAREDIR = $${INSTALL_ROOT}/share

doc.commands = doxygen \
    Doxyfile;

unix:INCLUDEPATH += \
    $${QUMBIA_TANGO_CONTROLS_INCLUDES}

unix:LIBS +=  \
    -L$${QUMBIA_TANGO_CONTROLS_LIBDIR} \
    -l$${QUMBIA_TANGO_CONTROLS_LIB}

# need to adjust qwt path
!packagesExist($${QWT_PKGCONFIG}){
    unix:INCLUDEPATH += $${QWT_INCLUDES} $${QWT_INCLUDES_USR}
    unix:LIBS += -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QTVER_SUFFIX}
}
