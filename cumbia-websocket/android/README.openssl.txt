As stated in http://doc.qt.io/qt-5/opensslsupport.html :

"Adding OpenSSL Support for Android

The Qt installation package comes with OpenSSL support but the OpenSSL libraries are not part of
the package due to legal restrictions in some countries. If your application depends on OpenSSL,
consider packaging the SSL libraries with your Application Package (APK) as the target device
may or may not have them."

The instructions in that document are correct but not up to date.

As to 2018 08 02, you need:

* openssl-1.0.x series, not 1.1.x
* openssl-1.0.x builds with android NDK <= android-ndk-r15c  (July 2017)
  (NDK Archives: https://developer.android.com/ndk/downloads/older_releases)

Files in this folder: Setenv-android.sh
Use it from within the openssl-1.0.x source folder by invoking

.  ../Setenv-android.sh

Then proceed according to the instructions in the aforementioned link:
http://doc.qt.io/qt-5/opensslsupport.html.

$ cd cumbia-libs/cumbia-websocket/android/
$ wget  https://www.openssl.org/source/openssl-1.0.2o.tar.gz
$ tar xzf openssl-1.0.2o.tar.gz
$ cd openssl-1.0.2o/

Edit Setenv-android.sh.

According to the NDK version used, set

_ANDROID_NDK="android-ndk-r15c"

check

_ANDROID_EABI="arm-linux-androideabi-4.9"

and select the highest available android api for the given NDK version used:

_ANDROID_API="android-26"

For example
$ ls /usr/local/android-ndk-r15c/platforms/

shows these subdirs:

android-12  android-13  [...]  android-19  android-21  android-23  android-24  android-3  android-4  android-5  android-8  android-9

and so just choose android-26

Supposing that Android NDK is installed under /usr/local/android-ndk-r10e

$ export ANDROID_NDK_ROOT=/usr/local/android-ndk-r15c
$ .  ../Setenv-android.sh
$ ./Configure android shared
$ make -j5

Once the library is built, you must add it to the Android project from Qt creator -> Projects -> Android -> build ->
"Additional libraries".

The libraries to add can be found within the openssl-1.0.2o (or whatever version) sources:
1.  libcrypto.so
2.  libssl.so

This results in a section in the .pro file similar to this:

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia-qtcontrols-qml-qt5plugin.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia-qtcontrols-qt5.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia-websocket.so \

        ###
        ### Here libcrypto.so and libssl.so are included in the application package (APK)
        ###
        $$PWD/../../../../cumbia-websocket/android/openssl-1.0.2o/libcrypto.so \
        $$PWD/../../../../cumbia-websocket/android/openssl-1.0.2o/libssl.so
}



