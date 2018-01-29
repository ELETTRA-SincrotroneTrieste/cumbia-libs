include(../../qumbia-tango-controls/qumbia-tango-controls.pri)
include(../../qumbia-epics-controls/qumbia-epics-controls.pri)


INSTALL_ROOT = /usr/local

DESIGNER_PLUGIN_DIR=$${INSTALL_ROOT}/lib/plugins/designer

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets x11extras printsupport
    QT += designer
} else {
    CONFIG += plugin
    CONFIG += designer
}

CONFIG += debug

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

TEMPLATE = lib

TARGET = designer/$${QUMBIA_TANGO_CONTROLS_LIB}-plugin

CONFIG	+= qt thread warn_on

# Debug or release?
CONFIG += debug

unix:LIBS += -L.. 

# the same goes for INCLUDEPATH
INCLUDEPATH -= $${INC_DIR}
INCLUDEPATH += ../src . ./forms

target.path = $${DESIGNER_PLUGIN_DIR}

INSTALLS += target


