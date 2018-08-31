include($INCLUDE_DIR$/cumbia-qtcontrols/cumbia-qtcontrols.pri)
include($INCLUDE_DIR$/cumbia-websocket/cumbia-websocket.pri)

QT += quick
CONFIG += c++11

TEMPLATE = app
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -L/libs/armeabi-v7a


contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        /libs/armeabi-v7a/libcumbia.so \
        /libs/armeabi-v7a/libcumbia-qtcontrols-qt5.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../cumbia-websocket/android/openssl-lib/libcrypto.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../cumbia-websocket/android/openssl-lib/libssl.so \
        /libs/armeabi-v7a/libcumbia-websocket-qt5.so \
        $$PWD/../../../../../../../../../../../usr/local/qt-5.11.1/5.11.1/android_armv7/qml/eu/elettra/cumbiaqmlcontrols/libcumbia-qtcontrols-qml-qt5plugin.so
}

LIBS -= -lqwt -lpthread
