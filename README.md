# cumbia-libs

## Introduction
Cumbia is a new library that offers a carefree approach to multi-threaded application design and implementation.
Written from scratch, it can be seen as the evolution of the QTango library, because it offers a more flexible
and object oriented multi-threaded programming style. Less concern about locking techniques and synchronization,
and well defined design patterns stand for more focus on the work to be performed inside Cumbia Activities and
reliable and reusable software as a result. The user writes Activities and decides when their instances are started
and to which thread they belong. A token is used to register an Activity, and activities with the same token are run
in the same thread. Computed results can be forwarded to the main execution thread, where a GUI can be updated.
In conjunction with the Cumbia-Tango module, this framework serves the developer willing to connect an application
to the Tango control system. The integration is possible both on the client and the server side. An example of a
TANGO device using Cumbia to do work in background has already been developed, as well as simple Qt graphical
clients relying on the framework. An Epics module has been added to the cumbia-libs set.


## Download and installation


### Download the cumbia-libs set of modules

> git clone https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git

### Check pkgconfig and dependencies

Most of the software relies on *pkgconfig* for its configuration. Please be sure that the
PKG_CONFIG_PATH includes the relevant dependencies required by cumbia. For example:

> echo $PKG_CONFIG_PATH
> /usr/local/lib/pkgconfig:/usr/local/tango-9.2.5a/lib/pkgconfig:/usr/local/omniorb-4.2.1/lib/pkgconfig:/usr/local/zeromq-4.0.7/lib/pkgconfig:/home/giacomo/devel/epics/base-3.16.1/lib/pkgconfig:/usr/local/qwt-6.1.3/lib/pkgconfig

shows the list of the required packages to build all the modules in cumbia-libs: omniorb, tango, zeromq, epics qwt...

### 1. Install cumbia base library

> cd cumbia

> ./configure --prefix=/usr/local --includedir=/usr/local/include/cumbia

> make && make install

#### 1b. Install the documentation
> make doc

This generates the documentation under the doc/ subdirectory. To install it under
/usr/local/share/doc/cumbia execute:

> make install-html

### 2. Install the cumbia-tango module if you want to connect to the Tango control system

> cd ../cumbia-tango

> ./configure --prefix=/usr/local --includedir=/usr/local/include/cumbia-tango

> make && make install

*Note* if you installed cumbia outside a system wide directory, you may have to
update the PKG_CONFIG_PATH environment so that it contains cumbia's lib/pkgconfig
directory containing the *cumbia.pc* file. For example:

> export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/giacomo/devel/usr/local/lib/pkgconfig

#### 2b. Install the documentation
> make doc
This generates the documentation under the doc/ subdirectory. To install it under
/usr/local/share/doc/cumbia-tango execute:

> make install-html

Next, install the cumbia qtcontrols module: a set of widgets to compose graphical user interfaces
that understand cumbia.

### 3. The cumbia-qtcontrols module

This module provides a set of Qt widgets aware of the cumbia data model. cumbia-qtcontrols is independent
from the actual engine (Tango or Epics) that is used to connect to a control system.

In order to build this module, you need the Qt and [Qwt libraries]
(http://qwt.sourceforge.net/). If you build Qwt from sources, remember to uncomment (enable) the line

>  QWT_CONFIG     += QwtPkgConfig

in the *qwtconfig.pri* Qwt build configuration file. Then you can proceed to building the cumbia-qtcontrols module
as follows:

> cd ../cumbia-qtcontrols

Set the install path for the cumbia-qtcontrols module in cumbia-qtcontrols.pri (example: /usr/local):

> INSTALL_ROOT = /usr/local

> qmake

If the option
>  QWT_CONFIG     += QwtPkgConfig
in  the *qwtconfig.pri* Qwt  configuration file has been enable, qmake should print a message like this:

> Project MESSAGE: Qwt: using pkg-config to configure qwt includes and libraries (Qt5Qwt6)

Otherwise, it may be necessary to adjust the cumbia-qtcontrols.pri configuration file in order to specify
where Qwt is installed. The relevant configuration directives to look for are the following:

> QWT_HOME=  QWT_INCLUDES=  and maybe QWT_HOME_USR

Inspect cumbia-qtcontrols.pri  to see where and how these are used.

Once the

> INSTALL_ROOT=

directive has been set, you can build and install cumbia-qtcontrols.

> make && make install

will finally install the libraries (and the documentation, with no extra commands this time)
in the location specified by the directive

> INSTALL_ROOT

in cumbia-qtcontrols.pri


### 4. The qumbia-tango-controls module

*qumbia-tango-controls* glues the *cumbia-qtcontrols* module and the *cumbia-tango* engine together.

> cd ../qumbia-tango-controls

See the considerations done for the *cumbia-qtcontrols* Qwt and cumbia-qtcontrols.pri configuration options
and then proceed the same way to build the library and install the documentation:

> qmake
> make && make install

### 5. cumbia-epics.

If you rely on the Epics control system, please install this module as follows:

> cd ../cumbia-epics
> ./configure --prefix=/usr/local --includedir=/usr/local/include/cumbia-epics
> make && make install

> make doc && make install-html

##### Note
Check that the PKG_CONFIG_PATH contains the epics-base lib/pkgconfig directory.


### 6. qumbia-epics-controls

The *qumbia-epics-controls* module integrates *cumbia-epics* with *cumbia-qtcontrols*.

#### Installation



> cd ../qumbia-epics-controls

The same care needed for the *cumbia-qtcontrols* Qwt and cumbia-qtcontrols.pri configuration must be taken.
Then build the library and install the documentation:

> qmake
> make && make install

### 7. Plugins

#### 7a. cumbia-dbus - DBus plugin for cumbia

> cd ../qumbia-plugins/cumbia-dbus

Adjust cumbia-dbus.pro *include()* directive if necessary

> qmake

> make

> make install

#### 7b. cumbia-multiread - Multiple reader for cumbia

cd ../cumbia-multiread

Adjust cumbia-multiread.pro if necessary

> qmake

> make

> make install

#### 7c. Qt designer plugin

> cd ../qt-designer/


### 8. Build some basic applications

> cd ../qumbia-apps

Some base tools are provided to test the cumbia-libs and perform some basic operations on the
underlying control system (reading or writing quantities).

> cd generic_client

Check generic_client.pro and adjust the include directives if needed.
Beware that the lines

> CONFIG+=link_pkgconfig  and PKGCONFIG +=

tell Qt to rely on pkgconfig for dependency resolution. See the comments on the *cumbia-qtcontrols* section above.

To build generic_client type

> qmake
> make

Read the README.txt for details

> ./generic_client test/device/1/double_scalar,giacomo:ai1,test/device/1/double_spectrum_ro

starts the generic_client and performs readings on the two Tango attributes specified and the Epics PV giacomo:ai1

# Contacts

[Giacomo Strangolino] (mailto:giacomo.strangolino@elettra.eu)




