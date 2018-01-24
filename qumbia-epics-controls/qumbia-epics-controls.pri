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
# Here qumbia-epics-controls will be installed
    INSTALL_ROOT = /usr/local
#
#
# Here qumbia-epics-controls include files will be installed
    QUMBIA_EPICS_CONTROLS_INCLUDES=$${INSTALL_ROOT}/include/qumbia-epics-controls
#
#
# Here qumbia-epics-controls share files will be installed
#
    QUMBIA_EPICS_CONTROLS_SHAREDIR=$${INSTALL_ROOT}/share/qumbia-epics-controls
#
#
# Here qumbia-epics-controls libraries will be installed
    QUMBIA_EPICS_CONTROLS_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-epics-controls documentation will be installed
    QUMBIA_EPICS_CONTROLS_DOCDIR=$${INSTALL_ROOT}/share/doc/qumbia-epics-controls
#
# The name of the library
    QUMBIA_EPICS_CONTROLS_LIB=qumbia-epics-controls$${QTVER_SUFFIX}
#
#
#
# ======================== DEPENDENCIES =================================================
#
# Qwt libraries (>= 6.1.2) are installed here:
#   QWT_HOME =
#
exists(/usr/local/qwt-6.1.4) {
    QWT_HOME = /usr/local/qwt-6.1.4
}
exists(/usr/local/qwt-6.1.3) {
    QWT_HOME = /usr/local/qwt-6.1.3
}
exists(/usr/local/qwt-6.1.2) {
    QWT_HOME = /usr/local/qwt-6.1.2
}

QWT_LIB = qwt

QWT_INCLUDES=$${QWT_HOME}/include

QWT_HOME_USR = /usr
QWT_INCLUDES_USR = $${QWT_HOME_USR}/include/qwt

CONFIG+=link_pkgconfig
PKGCONFIG += cumbia cumbia-qtcontrols$${QTVER_SUFFIX}

packagesExist(epics-base-linux-x86_64) {
    PKGCONFIG += epics-base-linux-x86_64
    DEFINES += EPICS=1
}
else {
    message("package epics-base-linux-x86_64 not found")
}

packagesExist(cumbia-epics) {
    PKGCONFIG += cumbia-epics
    DEFINES += CUMBIA_EPICS=1
}
else {
    message("package cumbia-epics not found")
}

packagesExist(qumbia-epics-controls$${QTVER_SUFFIX}) {
    PKGCONFIG += qumbia-epics-controls$${QTVER_SUFFIX}
    DEFINES += QUMBIA_EPICS_CONTROLS=1
}
else {
    message("package cumbia-epics-controls not found")
}


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

DEFINES += QUMBIA_EPICS_CONTROLS_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QUMBIA_EPICS_CONTROLS_VERSION=$${VERSION_HEX} \
    VER_MAJ=$${VER_MAJ} \
    VER_MIN=$${VER_MIN} \
    VER_FIX=$${VER_FIX}

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

packagesExist(qumbia-epics-controls$${QTVER_SUFFIX}) {
message("Linikgin")
unix:INCLUDEPATH += \
    $${QUMBIA_EPICS_CONTROLS_INCLUDES}

unix:LIBS +=  \
    -L$${QUMBIA_EPICS_CONTROLS_LIBDIR} \
    -l$${QUMBIA_EPICS_CONTROLS_LIB} \
    -lca
}
else {
message("not linking")
}

# need to adjust qwt path
!packagesExist($${QWT_PKGCONFIG}){
    unix:INCLUDEPATH += $${QWT_INCLUDES} $${QWT_INCLUDES_USR}
    unix:LIBS += -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QTVER_SUFFIX}
}
