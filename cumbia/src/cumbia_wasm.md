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

Qt and cumbia libraries have been tested with chromium browser, the Emscripten version 1.38.30 and Qt 5.13

> ./emsdk install emscripten-1.38.30  emscripten-tag-1.38.30-32bit  binaryen-tag-1.38.30-64bit  sdk-fastcomp-tag-1.38.30-64bit sdk-fastcomp-1.38.30-64bit

> ./emsdk activate binaryen-tag-1.38.30-64bit  sdk-fastcomp-tag-1.38.30-64bit sdk-fastcomp-1.38.30-64bit

#### Qt build and installation

> cd ~/devel

> git clone  https://github.com/qt/qt5.git

> cd qt

> git checkout 5.13.2

> ./init-repository

Set the environment for *emscripten*

> source ~/devel/emsdk/emsdk_env.sh

Configure Qt with *-feature-thread*, skip tests and examples
Further options can be found executing  *./configure --help*

> ./configure -xplatform wasm-emscripten -prefix /usr/local/qt-5.13.2-wasm -feature-thread -skip qtwebengine -nomake examples  -nomake tests -opensource -confirm-license  && make install

> make -j9

If you prefer, only a subset of modules can be built:  *make module-qtbase module-qtdeclarative [other modules]*

> [sudo] make install




