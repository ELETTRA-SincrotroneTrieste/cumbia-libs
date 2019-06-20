# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here qumbia-qtcontrols will be installed
# INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
#

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

#
#
# Here qumbia-plugins include files will be installed
    QUMBIA_PLUGINS_INCLUDES=$${INSTALL_ROOT}/include/qumbia-plugins
#
#
# Here qumbia-plugins share files will be installed
#
    QUMBIA_PLUGINS_SHARE=$${INSTALL_ROOT}/share/qumbia-plugins
#
#
# Here qumbia-plugins libraries will be installed
    QUMBIA_PLUGINS_LIBDIR=$${INSTALL_ROOT}/lib/qumbia-plugins
#
#
# Here qumbia-plugins documentation will be installed
    QUMBIA_PLUGINS_DOCDIR=$${INSTALL_ROOT}/share/doc/qumbia-plugins
#
#
# + ----------------------------------------------------------------- +
#

INC_DIR = $${QUMBIA_PLUGINS_INCLUDES}
SHAREDIR = $${QUMBIA_PLUGINS_SHARE}
PLUGIN_LIB_DIR = $${QUMBIA_PLUGINS_LIBDIR}

DESTDIR = plugins


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

unix:!android-g++ {
    DEFINES += CUMBIAQTCONTROLS_HAS_QWT=1
}

DOC_DIR = $${QUMBIA_PLUGINS_DOCDIR}
doc.commands = \
    doxygen \
    Doxyfile;
doc.files = doc/*
doc.path = $${DOC_DIR}

QMAKE_EXTRA_TARGETS += doc

# lib

target.path = $${PLUGIN_LIB_DIR}
inc.path = $${INC_DIR}

# installation

INSTALLS += target \
    inc \
    doc

DISTFILES += \
    $$PWD/qumbia-plugins.md
