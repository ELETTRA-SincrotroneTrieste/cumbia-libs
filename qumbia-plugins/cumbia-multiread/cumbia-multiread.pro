include(../qumbia-plugins.pri)
include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

TARGET = cumbia-multiread-plugin
TEMPLATE = lib
CONFIG += plugin debug silent

SOURCES += \
    qumultireader.cpp

HEADERS += \
    qumultireader.h
DISTFILES += cumbia-multiread.json 

inc.files += $${HEADERS}


# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
