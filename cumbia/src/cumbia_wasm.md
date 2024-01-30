# Qt and cumbia for WebAssembly {#cumbia_wasm}

In this section, we will learn how to build Qt and cumbia for WebAssembly.
To start write and build applications for WebAssembly, please skip to
<a href="../../cumbia-websocket/html/cumbia_ws_wasm.html">this page</a>.


## Build multi threaded Qt for WebAssembly.

At the moment of writing this documentation, precompiled Qt binaries do not support multi threaded applications.
Qt must be built from source.
The official documentation is available <a href="https://doc.qt.io/qt-5/wasm.html">here</a>.
It contains basic information and further links, including a <a href="https://wiki.qt.io/Qt_for_WebAssembly">wiki page</a>.

### Emscripten installation

> cd ~/devel

> git clone https://github.com/emscripten-core/emsdk.git

> cd emsdk

Qt and cumbia libraries have been tested with chromium browser, the Emscripten version 1.38.47 and Qt 5.14

> ./emsdk install emscripten-1.38.47-fastcomp

> ./emsdk activate 1.38.47-fastcomp

You may want to install multiple versions of the SDK, in which case the "--embedded" option is useful.
It locates all configuration and cache files inside the SDK dir so that multiple versions do not conflict.

> ./emsdk activate --embedded 1.38.47-fastcomp


### Qt build and installation

> cd ~/devel

> git clone  https://github.com/qt/qt5.git

> cd qt

> git checkout 5.14.2

> ./init-repository

#### init-repository excluding some modules:

> ./init-repository --module-subset=all,-qtwayland,-qt,-qtwebengine,-qtpim,-qtquick3d,-qtmacextras

Set the environment for *emscripten*

> source ~/devel/emsdk/emsdk_env.sh

#### Command line arguments patch

Check whether the following patch has been applied or not: it is important in order to pass command line arguments to
applications

https://codereview.qt-project.org/c/qt/qtbase/+/248624/6/src/plugins/platforms/wasm/qtloader.js#420

#### Configure, build and install

Configure Qt with *-feature-thread*, skip tests and examples
Further options can be found executing  *./configure --help*

> ./configure -xplatform wasm-emscripten -prefix /usr/local/qt-5.14.2-wasm -feature-thread -skip qtwebengine -skip qtquick3d  -skip qtpurchasing -skip qtserialport   -nomake examples  -nomake tests -opensource -confirm-license  && make -j9 && make install

> make -j9

If you prefer, only a subset of modules can be built:  *make module-qtbase module-qtdeclarative [other modules]*

> [sudo] make install

### Cumbia build and installation

#### Set the environment for *emscripten*

> source ~/devel/emsdk/emsdk_env.sh

#### Clone cumbia-libs

> git clone https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git

#### Build

The modules that we will build for WebAssembly are the following:

- cumbia (base c++ library, threads, services)
- cumbia-qtcontrols (widgets and their multi engine support)
- cumbia-websocket (websocket engine)
- cumbia-random [optional]

In the example installation below, we opt for an installation of the libraries under *prefix/lib/wasm*,
while include files are fully compatible across cross-builds and can be installed over a previous C++
installation. If the only static library in your cumbia install environment is WebAssembly's, then *prefix/lib* can be
shared across dynamic C++ shared libraries and *wasm* static library. In this case, the *.pro* and *.pri*
files used by the Qt modules discussed below need to be adjusted.


##### cumbia


> cd cumbia-libs/cumbia

Amongst others, you will find these files:

- meson.build -> meson.build.cpp (default links to the cpp shared library build file)
- meson.build.cpp
- meson.build.wasm
- wasm-cross.txt  (the meson cross file)

##### Check paths in wasm-cross.txt
Edit wasm-cross.txt and check the following lines:

c = '/home/test/devel/emsdk/fastcomp/emscripten/emcc'
cpp = '/home/test/devel/emsdk/fastcomp/emscripten/em++'
ar = '/home/test/devel/emsdk/fastcomp/emscripten/emar'

to make sure they match the emsdk installation paths


Use meson.build.wasm:

> ln -sf meson.build.wasm  meson.build

Run meson and output to build-wasm

> meson --cross-file=wasm-cross.txt  build-wasm

> cd build-wasm

Configure install prefix and lib dir.
In this example, we want an installation in the user's home.

> meson configure -Dprefix=/home/test/.local/cumbia-libs
> meson configure -Dlibdir=lib/wasm

> ninja && ninja install


##### cumbia-qtcontrols

Move from cumbia/build-wasm into cumbia-qtcontrols

> cd ../../cumbia-qtcontrols

Execute the qmake from the Qt for WebAssembly installation. In this example we assume qt was installed under
the .local directory in the user's home.

Make sure the *prefix* is the same used for the *cumbia* installation above. qmake accepts INSTALL_ROOT to
configure the prefix. cumbia base libraries are searched under

> /home/test/.local/qt-5.14.2-wasm/bin/qmake  INSTALL_ROOT=/home/test/.local/cumbia-libs

You should read a message like this:

```bash
Project MESSAGE: cumbia-qtcontrols: building for WebAssembly
Project MESSAGE: Setting PTHREAD_POOL_SIZE to 4
Project MESSAGE: Setting TOTAL_MEMORY to 1GB
bash
```

##### Note

Applications should set the expected number of concurrent threads at build time.
This can be done either by setting QMAKE_WASM_PTHREAD_POOL_SIZE in the .pro file or
passing the *QMAKE_WASM_PTHREAD_POOL_SIZE=x* option to *qmake*. This maps to Emscripten PTHREAD_POOL_SIZE option.

>  /home/test/.local/qt-5.14.2-wasm/bin/qmake  INSTALL_ROOT=/home/test/.local/cumbia-libs QMAKE_WASM_PTHREAD_POOL_SIZE=12

Please refer to the <a href="https://wiki.qt.io/Qt_for_WebAssembly#Multithreading_Support">Multithreading Support</a>
section in the <a href="https://wiki.qt.io/Qt_for_WebAssembly">Qt for WebAssembly</a> page for additional details.

Now you can *build cumbia-qtcontrols*:

> make -j9 && make install


##### cumbia-websocket and cumbia-random

Exactly the same procedure can be followed for the other two modules that need to be compiled (the latter is optional)

*websocket module*

> cd ../cumbia-websocket

> /home/test/.local/qt-5.14.2-wasm/bin/qmake  INSTALL_ROOT=/home/test/.local/cumbia-libs

> make && make install

*random module*, for testing purposes

> cd ../cumbia-random/

> /home/test/.local/qt-5.14.2-wasm/bin/qmake  INSTALL_ROOT=/home/test/.local/cumbia-libs

> make && make install

### Prepare applications for the web.

#### Notes

Remember that Tango and EPICS modules haven't been built. This means that you need to access those systems
through a proxy. A websocket proxy server can be downloaded from

> https://github.com/ELETTRA-SincrotroneTrieste/cumbia-websocket-proxy-server.git

It is a small Qt project uniquely aimed at testing websocket based C++ and WebAssembly applications and not
intended for production.

Plugins that are available for C++ and are dynamically loaded at runtime (dbus, introspection, historical db, formula)
are not suitable for WebAssembly builds.

#### Next steps

With these modules ready, you can now start writing and building applications for WebAssembly. Refer to
<a href="../../cumbia-websocket/html/cumbia_ws_wasm.html">this page</a> for details.
