include(../qumbia-plugins.pri)
include(../../qumbia-tango-controls/qumbia-tango-controls.pri)
include(../../qumbia-epics-controls/qumbia-epics-controls.pri)

# The application will be installed under INSTALL_ROOT (i.e. prefix)
#
# WARNING: INSTALL_ROOT is defined by qumbia-plugins.pri, qumbia-epics-controls.pri and qumbia-tango-controls.pri
# The first definition in qumbia-plugins.pri is used.
#
#
# To set the prefix at build time, call 
# qmake   "INSTALL_ROOT=/my/custom/path"
#
isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

DESIGNER_PLUGIN_DIR=$${PLUGIN_LIB_DIR}/designer

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets x11extras printsupport
    QT += designer
} else {
    CONFIG += plugin
    CONFIG += designer
}

TEMPLATE = lib
TARGET = designer/cumbia-qtcontrols-pool-plugin
CONFIG	+= qt thread warn_on debug

lessThan(QT_MAJOR_VERSION, 5) {
	CONFIG += designer
}

FORMS += \
    forms/editsourcetargetdialog.ui \
    forms/editsourcedialog.ui \
    forms/ttablebooleditor.ui \
    forms/tlabelbooleditor.ui \
    forms/estringlisteditor.ui \
    forms/tledbooleditor.ui

SOURCES	+= \
    forms/edittargetdialog.cpp \
    forms/editsourcedialog.cpp \
    forms/ttablebooleditor.cpp \
    forms/tlabelbooleditor.cpp \
    forms/estringlisteditor.cpp \
    pointeditor.cpp \
    cucontrolsplugin.cpp

HEADERS	+= \
    forms/edittargetdialog.h \
    forms/ttablebooleditor.h \
    forms/editsourcedialog.h \
    forms/tlabelbooleditor.h \
    forms/estringlisteditor.h \
    pointeditor.h \
    cucontrolsplugin.h

RESOURCES += cutangoqtcontrolsplugin.qrc


unix:LIBS += -L.. 

# the same goes for INCLUDEPATH
INCLUDEPATH -= $${INC_DIR}
INCLUDEPATH += ../src . ./forms

target.path = $${DESIGNER_PLUGIN_DIR}

INSTALLS += target


# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
