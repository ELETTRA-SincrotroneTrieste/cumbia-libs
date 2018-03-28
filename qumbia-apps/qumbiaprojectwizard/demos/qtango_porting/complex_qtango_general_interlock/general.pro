include(/usr/local/qtango/include/qtango6/qtango.pri)

CONFIG += debug
QT += sql
QT += phonon

DEFINES -= QT_NO_DEBUG_OUTPUT

RESOURCES += general.qrc

# DEFINES -= QT_NO_DEBUG_OUTPUT


SOURCES += src/tlabelitem.cpp \
           src/ttreewidget.cpp \
           src/ttablewidget.cpp \
           src/interlock.cpp \
           src/main.cpp \
           src/queryconfiguration.cpp \
           src/mysqlmodel.cpp \
    src/ttreeview.cpp \
    src/Sound.cpp \
    src/tabwidget.cpp \
    src/treeitem.cpp

HEADERS += src/tlabelitem.h \
           src/ttreewidget.h \
           src/ttablewidget.h \
           src/interlock.h \
           src/queryconfiguration.h \
           src/mysqlmodel.h \
    src/ttreeview.h \
    src/Sound.h \
    src/tabwidget.h \
    src/treeitem.h


FORMS    = src/interlock.ui src/queryconfiguration.ui

TARGET   = bin/ferminterlock

sounds.files = sounds/generalintlkpanel_beep.ogg
sounds.path = $${INSTALL_ROOT}/share/sounds

INSTALLS += sounds

OTHER_FILES += \
    properties.txt
