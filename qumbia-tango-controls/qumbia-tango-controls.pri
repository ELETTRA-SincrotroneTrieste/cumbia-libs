linux|freebsd|openbsd{

    exists(../cumbia-qt.prf) {
        include(../cumbia-qt.prf)
    }

    # + ----------------------------------------------------------------- +
    #
    # Customization section:
    #
    # Customize the following paths according to your installation:
    #
    #
    # Here qumbia-tango-controls will be installed
    # INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
    #

    isEmpty(INSTALL_ROOT) {
        INSTALL_ROOT = /usr/local/cumbia-libs
    }

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

    CONFIG += c++17 link_pkgconfig
    PKGCONFIG += tango
    PKGCONFIG += x11

    packagesExist(cumbia) {
        PKGCONFIG += cumbia
    }

    packagesExist(cumbia-tango) {
        PKGCONFIG += cumbia-tango
    }

    packagesExist(cumbia-qtcontrols$${QTVER_SUFFIX}) {
     PKGCONFIG += cumbia-qtcontrols$${QTVER_SUFFIX}
    }

    packagesExist(qumbia-tango-controls$${QTVER_SUFFIX}) {
     PKGCONFIG += qumbia-tango-controls$${QTVER_SUFFIX}
    }

    TEMPLATE = app
    QT +=  core gui widgets


    QWT_PKGCONFIG =

    packagesExist(qwt){
        PKGCONFIG += qwt
        QWT_PKGCONFIG = qwt
    }
    else:packagesExist(Qt5Qwt6){
        PKGCONFIG += Qt5Qwt6
        QWT_PKGCONFIG = Qt5Qwt6
    } else {
        warning("Qwt: no pkg-config file found")
        warning("Qwt: export PKG_CONFIG_PATH=/usr/path/to/qwt/lib/pkgconfig if you want to enable pkg-config for qwt")
        warning("Qwt: if you build and install qwt from sources, be sure to uncomment/enable ")
        warning("Qwt: QWT_CONFIG     += QwtPkgConfig in qwtconfig.pri qwt project configuration file")
    }

    DEFINES += CUMBIA_DEBUG_OUTPUT

    VERSION_HEX = 0x010502
    VERSION = 1.5.2

    DEFINES += QUMBIA_TANGO_CONTROLS_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
        QUMBIA_TANGO_CONTROLS_VERSION=$${VERSION_HEX}

    QMAKE_CXXFLAGS += -std=c++17  -Wall
    CONFIG += c++17

    freebsd-g++ {
        message( )
        message( *)
        message( * Compiling under FreeBSD)
        message( * :-P)
        message( )
        unix:LIBS -= -ldl
    }

    MOC_DIR = moc
# don't call it obj! (BSD!)
    OBJECTS_DIR = objs
    FORMS_DIR = ui
    LANGUAGE = C++
    UI_DIR = src
    QMAKE_DEL_FILE = rm \
        -rf
    QMAKE_CLEAN = moc \
        obj \
        Makefile \
        *.tag

    SHAREDIR = $${INSTALL_ROOT}/share

    unix:INCLUDEPATH += \
        $${QUMBIA_TANGO_CONTROLS_INCLUDES}

    unix:LIBS +=  \
        -L$${QUMBIA_TANGO_CONTROLS_LIBDIR} \
        -l$${QUMBIA_TANGO_CONTROLS_LIB}

    # need to adjust qwt path

    isEmpty(QWT_PKGCONFIG){

        lessThan(QT_MAJOR_VERSION, 5) {
            QWT_QTVER_SUFFIX =
        } else {
            QWT_QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
        }

        message("no Qwt pkg-config file found")
        message("adding $${QWT_INCLUDES} and $${QWT_INCLUDES_USR} to include path")
        message("adding  -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QWT_QTVER_SUFFIX} to libs")
        message("this should work for ubuntu installations")

        unix:INCLUDEPATH += $${QWT_INCLUDES} $${QWT_INCLUDES_USR}
        unix:LIBS += -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QWT_QTVER_SUFFIX}
    }

} # linux-g++|freebsd-g++
