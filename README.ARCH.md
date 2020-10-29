pacman -Ss make meson ninja wget cmake pkgconfig

pacman -S qt


1. ZEROMQ

git clone https://github.com/zeromq/libzmq.git

cd libzmq/

mkdir build && cd build

cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/zeromq

make -j9 && sudo make install

git clone https://github.com/zeromq/cppzmq.git
cd cppzmq && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/zeromq-4.3.4  &amp;&amp; make -j9



2. OMNIORB
[giacomo@woody omniORB-4.2.3]$ ./configure --prefix=/usr/local/omniorb-4.2.3
make -j9 && sudo make install

3. EPICS
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
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.3.3
sudo make install

cd ../../cppTango/

mkdir build && cd build

cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tango-9.3.3/ -DZMQ_BASE=/usr/local/zeromq-4.3.4 -DCPPZMQ_BASE=/usr/local/zeromq-4.3.4 -DOMNIIDL_PATH=/usr/local/omniorb-4.2.4/bin/ -DOMNI_BASE=/usr/local/omniorb-4.2.4 -DIDL_BASE=/usr/local/tango-9.3.3

sudo make install

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
export PKG_CONFIG_PATH=/usr/local/qwt-6.1.5/lib/pkgconfig:/usr/local/tango-9.3.3/lib/pkgconfig:/usr/local/omniorb-4.2.4/lib/pkgconfig:/usr/local/zeromq-4.3.4/lib/pkgconfig:/usr/local/cumbia-libs/lib/pkgconfig

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
