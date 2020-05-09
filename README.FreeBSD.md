## Cumbia libs installation on FreeBSD

Starting point: a fresh FreeBSD desktop installation

Prerequisites

> pkg install git cmake cvs python3

1. ZEROMQ

Configuration script warns that sodium library is missing:

> pkg install libsodium

> git clone https://github.com/zeromq/libzmq.git
> cd libzmq/
> mkdir build && cd build

If you want to build tests, you will have to fix some includes. For now, disable them

> cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/zeromq -DBUILD_TESTS=off -DZMQ_BUILD_TESTS=off
> make -j9 && sudo make install

> git clone https://github.com/zeromq/cppzmq.git
> cd cppzmq && mkdir build && cd build
cmake .. && make -j9




2. OMNIORB

> pkg install omniorb

```bash
New packages to be INSTALLED:
        omniORB: 4.2.3_1

```

3. EPICS -- does not build
wget https://epics.anl.gov/download/base/base-7.0.3.1.tar.gz
tar xzf  base-7.0.3.1.tar.gz 
cd  base-7.0.3.1
make -j9


4. TANGO
*WARNING* Needs Python2 in path

git clone https://github.com/tango-controls/tango-idl.git
git clone https://github.com/tango-controls/cppTango.git

cd tango-idl/
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.4.0
sudo make install

> cd ../../cppTango/
> mkdir build && cd build
> cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.4.0 -DZMQ_BASE=/usr/local/zeromq -DCPPZMQ_BASE=/usr/local/zeromq  -DOMNIIDL_PATH=/usr/local -DOMNI_BASE=/usr/local -DIDL_BASE=/usr/local/tango-9.4.0  -DBUILD_TESTING=off
> make
> make install

QWT
Download latest qwt
unzip qwt-6.1.4.zip
cd qwt-6.1.4
edit qwtconfig.pri enable QwtPkgConfig
qmake && make -j9 && sudo make install


cumbia

export EPICS_BASE=/usr/local/epics/base-7.0.3.1
export EPICS_HOST_ARCH=linux-x86_64
export PKG_CONFIG_PATH=/usr/local/qwt-6.1.4/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.3/lib/pkgconfig:/usr/local/zeromq/lib/pkgconfig

/scripts/cubuild.sh tango epics websocket random build

ADD cumbia-libs to pkgconfig
export PKG_CONFIG_PATH=/usr/local/qwt-6.1.4/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.3/lib/pkgconfig:/usr/local/zeromq/lib/pkgconfig:/usr/local/cumbia-libs/lib/pkgconfig

qutimearray3dplotplugin
qumbia-tango-findsrc-plugin

qmake && make -j9 && make install



hdbextractor C++ lib

> pacman -S mysql swig

> ./configure --prefix=/usr/local/hdbextractor
make -j9 && sudo make install

 export PKG_CONFIG_PATH=/usr/local/qwt-6.1.4/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.3/lib/pkgconfig:/usr/local/zeromq/lib/pkgconfig:/usr/local/cumbia-libs/lib/pkgconfig:/usr/local/hdbextractor/lib/pkgconfig



cumbia-historicaldb

meson  builddir

meson configure -Dprefix=/usr/local/cumbia-libs
cd bu
