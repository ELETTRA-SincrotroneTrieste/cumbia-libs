# Qt and cumbia for WebAssembly {#cumbia_wasm}

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

### Qt build and installation

> cd ~/devel

> git clone  https://github.com/qt/qt5.git

> cd qt

> git checkout 5.14.2

> ./init-repository

#### init-repository excluding some modules:

> ./init-repository --module-subset=all,-qtwayland,-qtx11extras,-qtwebengine,-qtpim,-qtquick3d,-qtmacextras

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




