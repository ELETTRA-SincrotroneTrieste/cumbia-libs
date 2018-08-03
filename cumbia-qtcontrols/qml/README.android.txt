Android building and installation (see also the IMPORTANT NOTE below)

Qt creator:

- Open Projects -> build and Run

- Click on Android for armeabi...

- Either
  a. In the build section, add  make install as "Custom process Step"
  b. run "make install" from the "build-cumbia-qtcontrols-qml-Android_for_armeabi_v7a_GCC_..." folder
    that will be created by the Qt creator build.

  The a. solution makes it possible to automatically install the library after building.

If the Qt installation is under

/usr/local/qt-5.11.1/      (just an example)

You should find the qml plugins under

 /usr/local/qt-5.11.1/5.11.1/android_armv7/qml/eu/elettra/cumbiaqmlcontrols/

For instance, `ls -ltr` on that folder gives:

ls /usr/local/qt-5.11.1/5.11.1/android_armv7/qml/eu/elettra/cumbiaqmlcontrols/ -ltr
total 2852
-rw-r--r-- 1 giacomo giacomo     599 Jul  2 10:13 cumbia-qtcontrols-qml.pri
-rw-r--r-- 1 giacomo giacomo     108 Jul 10 15:31 qmldir
-rw-r--r-- 1 giacomo giacomo    1739 Jul 10 16:49 CmdLineUtils.js
-rw-r--r-- 1 giacomo giacomo    1277 Jul 12 10:03 README.txt
-rw-r--r-- 1 giacomo giacomo     894 Jul 12 13:38 CumbiaDial.qml
-rw-r--r-- 1 giacomo giacomo     623 Jul 17 15:03 CuButton.qml
-rw-r--r-- 1 giacomo giacomo    1394 Jul 26 09:49 CumbiaCircularGauge.qml
-rw-r--r-- 1 giacomo giacomo    1233 Jul 26 09:49 CumbiaLabel.qml
-rw-r--r-- 1 giacomo giacomo   13846 Jul 26 09:49 CuTumbler.qml
-rw-r--r-- 1 giacomo giacomo     472 Jul 26 09:49 CuWButton.qml
-rw-r--r-- 1 giacomo giacomo   94736 Aug  2 18:29 plugins.qmltypes
-rw-r--r-- 1 giacomo giacomo    3871 Aug  3 09:22 CumbiaTrendChart.qml
-rw-r--r-- 1 giacomo giacomo    3276 Aug  3 09:22 CumbiaSpectrumChart.qml
-rwxr-xr-x 1 giacomo giacomo 2756112 Aug  3 09:28 libcumbia-qtcontrols-qml-qt5plugin.so
drwxr-xr-x 3 giacomo giacomo    4096 Aug  3 09:28 designer

========================================================================================
IMPORTANT NOTE:
========================================================================================

The application making use of this library must include it as dependency under
"Build Android APK" --> "Additional libraries"  (Qt creator)

"Build Android APK" --> "Additional libraries" must contain an entry like this, in our example:

/usr/local/qt-5.11.1/5.11.1/android_armv7/qml/eu/elettra/cumbiaqmlcontrols/libcumbia-qtcontrols-qml-qt5plugin.so

Typically, additional needed dependencies are:

- libcumbia.so
- libcumbia-qtcontrols-qt5.so

If the application makes use of the cumbia-websocket library, don't forget to add also:

- libcumbia-websocket-qt5.so as described in the README.android.txt under cumbia-websocket
- libssl dependencies as described in cumbia-websocket/android/README.openssl.txt
