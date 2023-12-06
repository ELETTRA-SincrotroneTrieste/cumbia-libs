include(../qumbia-plugins.pri)

include ($${INSTALL_ROOT}/include/quapps/quapps.pri)


QT += qml

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

isEmpty(prefix) {
    prefix=$${INSTALL_ROOT}
}

DEFINES += QT_NO_DEBUG_OUTPUT

DESIGNER_PLUGIN_INSTALL_DIR=$${PLUGIN_LIB_DIR}/designer

DEFINES += CUMBIA_QTCONTROLS_PLUGIN_DIR=\"\\\"$${prefix}/lib/qumbia-plugins\\\"\"

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets  printsupport uiplugin
    QT += designer
} else {
    CONFIG += designer
}

CONFIG += plugin
TEMPLATE = lib

TARGET = designer/cumbia-qtcontrols-pool-plugin
CONFIG	+= qt thread warn_on debug

CONFIG +=

lessThan(QT_MAJOR_VERSION, 5) {
	CONFIG += designer
}

FORMS += \
    forms/editsourcetargetdialog.ui \
    forms/editsourcedialog.ui \
    forms/editsourcewidget.ui \
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
    cucontrolsplugin.cpp \
    forms/editsourcewidget.cpp

HEADERS	+= \
    forms/edittargetdialog.h \
    forms/ttablebooleditor.h \
    forms/editsourcedialog.h \
    forms/tlabelbooleditor.h \
    forms/estringlisteditor.h \
    pointeditor.h \
    cucontrolsplugin.h \
    forms/editsourcewidget.h

RESOURCES += cutangoqtcontrolsplugin.qrc


unix:LIBS += -L.. 

# the same goes for INCLUDEPATH
INCLUDEPATH -= $${INC_DIR}
INCLUDEPATH += ../src . ./forms

target.path = $${DESIGNER_PLUGIN_INSTALL_DIR}
INSTALLS += target


# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
