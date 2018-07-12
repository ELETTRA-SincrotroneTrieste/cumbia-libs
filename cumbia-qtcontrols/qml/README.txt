NOTES:

1. Qt creator QML designer issues

1a.
   With the default system installation, as soon as you add a connection to the qml file, if you switch to edit mode and then
   back to design mode, the window disappears from the designer.
   FIX: Tools->Options->Qt Quick -> Qt Quick Designer tab: Use QML emulation layer that is built with the selected Qt
   and type the installation directory of the Qt libs, e.g. /home/user/.config/QtProject/qtcreator/qmlpuppet
   The option "Use fallback QML emulation layer" didn't work for me

   In my case, I used qtcreator from a binary version of Qt and built the emulation layer

1b.
   After writing a new C++ backend (QmlWriterBackend), I wrote a working example application.
   Nonetheless, switching from edit to design mode in Qt creator while creating the Qml element
   (a dial), the Qt creator qml designer did not show anything in the canvas. I got the following
   error on the command line:

    Process finished: ("{d82acdc5-c97d-4e42-8742-0c21c68a973b}", "rendermode", "-graphicssystem raster") exitCode: 0
    QAbstractSocket::waitForBytesWritten() is not allowed in UnconnectedState

   Solution: a the property "description" had the function definition in the header file but the CPP implementation
   was missing.

