## Cumbia libs installation on FreeBSD

Starting point: a fresh FreeBSD desktop installation

Prerequisites

> pkg install git cmake cvs python3 doxygen

Qt installation

1. ZEROMQ

1a. FreeBSD

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

1b. OpenBSD

Install zeromq from ports

> cd /usr/ports/net/zeromq

> make install clean

> cd /usr/ports/net/cppzmq

> make install clean

2. OMNIORB

#### 2a. FreeBSD

> pkg install omniorb

```bash
New packages to be INSTALLED:
        omniORB: 4.2.3_1

```

#### 2b. OpenBSD

Download omniorb and then

> tar xjf omniORB-4.3.0-b1.tar.bz2

> cd omniORB-4.3.0

> ./configure --prefix=/usr/local/omniorb-4.3.0

> gmake && gmake install



### 3. EPICS -- does not build
wget https://epics.anl.gov/download/base/base-7.0.3.1.tar.gz
tar xzf  base-7.0.3.1.tar.gz 
cd  base-7.0.3.1
make -j9


### 4. TANGO
*WARNING* Needs Python2 in path  (issue encountered on FreeBSD)

git clone https://github.com/tango-controls/tango-idl.git
git clone https://github.com/tango-controls/cppTango.git

> cd tango-idl/
> mkdir build
> cd build

> cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.4.0
> su
> make install

> cd ../../cppTango/
> mkdir build && cd build


#### 4a. FreeBSD

> cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.4.0 -DZMQ_BASE=/usr/local/zeromq -DCPPZMQ_BASE=/usr/local/zeromq  -DOMNIIDL_PATH=/usr/local -DOMNI_BASE=/usr/local -DIDL_BASE=/usr/local/tango-9.4.0  -DBUILD_TESTING=off

#### 4b. OpenBSD

In this case, zeromq was built through ports while omniorb was installed by hand. Just need to adjust the paths for the 
two.

> cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.4.0 -DZMQ_BASE=/usr/local -DCPPZMQ_BASE=/usr/local -DOMNIIDL_PATH=/usr/local/omniorb-4.3.0 -DOMNI_BASE=/usr/local/omniorb-4.3.0 -DIDL_BASE=/usr/local/tango-9.4.0  -DBUILD_TESTING=off

> make
> make install

### QWT

Download latest qwt

> unzip qwt-6.1.4.zip
> cd qwt-6.1.4

- edit qwtconfig.pri enable QwtPkgConfig
- edit qwtbuild.pri and change OBJECTS_DIR from obj to *objects*

```bash
OBJECTS_DIR       = objects

```

> qmake && make -j9
> su
> make install

## cumbia libs

EPICS at the moment cannot be built, so that the following two commands do not apply:

> export EPICS_BASE=/usr/local/epics/base-7.0.3.1
> export EPICS_HOST_ARCH=linux-x86_64

Set PKG_CONFIG_PATH appropriately:

> export PKG_CONFIG_PATH=/usr/local/qwt-6.1.4/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.3/lib/pkgconfig:/usr/local/zeromq/lib/pkgconfig

### The scripts/cubuild.sh way

The ./scripts/cubuild.sh script can be used. 

> bash ./scripts/cubuild.sh tango random websocket build

The manual way is described hereafter, and installs cumbia-libs under */usr/local/cumbia-libs*

### cumbia

> cd cumbia
> meson builddir
> cd builddir
> meson configure -Dprefix=/usr/local/cumbia-libs/ -Dlibdir=lib
> ninja
> su 
> ninja install

ADD cumbia-libs to pkgconfig
export PKG_CONFIG_PATH=/usr/local/qwt-6.1.4/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.3/lib/pkgconfig:/usr/local/zeromq/lib/pkgconfig:/usr/local/cumbia-libs/lib/pkgconfig

### cumbia-tango

Add */usr/local/cumbia-libs/libdata/pkgconfig/* to PKG_CONFIG_PATH

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/cumbia-libs/libdata/pkgconfig/

> cd ../../cumbia-tango
> meson builddir
> cd builddir
> meson configure -Dprefix=/usr/local/cumbia-libs/ -Dlibdir=lib
> ninja
> su 
> ninja install

### cumbia-qtcontrols

> cd ../../cumbia-qtcontrols

> qmake
```bash
Info: creating stash file /usr/home/giacomo/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/.qmake.stash
Project MESSAGE: cumbia-qtcontrols.pri: using pkg-config to configure qwt includes and libraries (Qt5Qwt6)
```

> make -j9
> su
> make install


qutimearray3dplotplugin
qumbia-tango-findsrc-plugin

qmake && make -j9 && make install


### cumbia-qtcontrols

> cd ../../qumbia-tango-controls

cumbia-qtcontrols installed *pkgconfig* file under */usr/local/cumbia

hdbextractor C++ lib

> pacman -S mysql swig

> ./configure --prefix=/usr/local/hdbextractor
make -j9 && sudo make install

 export PKG_CONFIG_PATH=/usr/local/qwt-6.1.4/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.3/lib/pkgconfig:/usr/local/zeromq/lib/pkgconfig:/usr/local/cumbia-libs/lib/pkgconfig:/usr/local/hdbextractor/lib/pkgconfig



cumbia-historicaldb

meson  builddir

meson configure -Dprefix=/usr/local/cumbia-libs
cd bu
