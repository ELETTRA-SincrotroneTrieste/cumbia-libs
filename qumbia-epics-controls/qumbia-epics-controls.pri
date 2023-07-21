linux-g++|freebsd-g++ {


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
    # Here qumbia-epics-controls will be installed

    isEmpty(INSTALL_ROOT) {
        INSTALL_ROOT = /usr/local/cumbia-libs
    }

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

    EPICS_BASE = $$(EPICS_BASE)
        isEmpty(EPICS_BASE) {
            error("qumbia-epics-controls.pri: EPICS_BASE environment variable is not set")
    } else {
            message("qumbia-epics-controls.pri: EPICS_BASE points to $${EPICS_BASE}")
    }

    EPICS_HOST_ARCH = $$(EPICS_HOST_ARCH)
        isEmpty(EPICS_HOST_ARCH) {
            error("qumbia-epics-controls.pri: EPICS_HOST_ARCH environment variable is not set")
    } else {
            message("qumbia-epics-controls.pri: EPICS_HOST_ARCH is defined as $${EPICS_HOST_ARCH}")
    }


    packagesExist(cumbia-epics) {
        PKGCONFIG += cumbia-epics
    }
    else {
        error("package cumbia-epics not found")
    }

    packagesExist(qumbia-epics-controls$${QTVER_SUFFIX}) {
        PKGCONFIG += qumbia-epics-controls$${QTVER_SUFFIX}
    }
    else {
        message("package cumbia-epics-controls not found")
    }


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


    VERSION_HEX = 0x010502
    VERSION = 1.5.2

    QMAKE_CXXFLAGS += -std=c++17 -Wall


    freebsd-g++ {
        message( )
        message( *)
        message( * Compiling under FreeBSD)
        message( * :-P)
        message( )
        unix:LIBS -= -ldl
        QMAKE_CXXFLAGS -= -std=c++0x
    }

    MOC_DIR = moc
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


    packagesExist(qumbia-epics-controls$${QTVER_SUFFIX} cumbia-epics  ) {

    !isEmpty(EPICS_BASE) {
            unix:INCLUDEPATH += $${EPICS_BASE}/include $${EPICS_BASE}/include/os/Linux  $${EPICS_BASE}/include/compiler/gcc \
                $${EPICS_BASE}/include/os/compiler/clang
            unix:LIBS+=-L$${EPICS_BASE}/lib/$${EPICS_HOST_ARCH}
    }

    DEFINES += CUMBIA_PRINTINFO

    DEFINES += QUMBIA_EPICS_CONTROLS

    DEFINES += QUMBIA_EPICS_CONTROLS_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
        QUMBIA_EPICS_CONTROLS_VERSION=$${VERSION_HEX}

    SHAREDIR = $${INSTALL_ROOT}/share

    unix:INCLUDEPATH += \
        $${QUMBIA_EPICS_CONTROLS_INCLUDES}

    unix:LIBS +=  \
        -L$${QUMBIA_EPICS_CONTROLS_LIBDIR} \
        -l$${QUMBIA_EPICS_CONTROLS_LIB} \
            -lca -lCom


    # need to adjust qwt path
    isEmpty(QWT_PKGCONFIG){
        message("no Qwt pkg-config file found")
        message("adding $${QWT_INCLUDES} and $${QWT_INCLUDES_USR} to include path")
        message("adding  -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QTVER_SUFFIX} to libs")
        message("this should work for ubuntu installations")

        unix:INCLUDEPATH += $${QWT_INCLUDES} $${QWT_INCLUDES_USR}
        unix:LIBS += -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QTVER_SUFFIX}
    }

    }
    else {
    }

} # linux-g++|freebsd-g++
