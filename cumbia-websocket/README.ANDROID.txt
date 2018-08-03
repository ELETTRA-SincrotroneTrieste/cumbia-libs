Android building and installation (see also the IMPORTANT NOTE below)

Qt creator:

- Open Projects -> build and Run

- Click on Android for armeabi...

- Either
  a. In the build section, add  make install as "Custom process Step"
  b. run "make install" from the "build-cumbia-websocket-Android_for_armeabi_v7a_GCC_..." folder
    that will be created by the Qt creator build.

  The a. solution makes it possible to automatically install the library after building from Qt creator

If the installation is successful, you should find a file named

libcumbia-websocket-qt5.so under

/libs/armeabi-v7a/

The same file can be found under the "build-cumbia-websocket-Android_for_armeabi_v7a_GCC_..." folder after
a successful build.

================================== IMPORTANT NOTE ===========================================

The application making use of this library must include libcumbia-websocket-qt5.so as dependency in Qt Creator in

"Build Android APK" --> "Additional libraries"  (Qt creator)

"Build Android APK" --> "Additional libraries" must contain an entry like this, in our example:

/libs/armeabi-v7a/libcumbia-websocket-qt5.so

Typically, additional needed dependencies are:

- libcumbia.so
- libcumbia-qtcontrols-qt5.so
- libcumbia-qtcontrols-qml-qt5plugin.so as described in the README.android.txt under cumbia-qtcontrols/qml
- libssl dependencies as described in cumbia-websocket/android/README.openssl.txt
