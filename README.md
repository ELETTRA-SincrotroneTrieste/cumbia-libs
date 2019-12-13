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

## Features

The base library supports several engines, including:

- Tango control system support https://www.tango-controls.org/
- EPICS control system support https://epics-controls.org/
- A test environment, named *cumbia-random*

## Apps

The default installation provides some base applications ready to be used with the supported engines

- *cumbia read* a command line reader featuring source names *auto completion* (1)
- *cumbia monitor* a command line monitor featuring source names *auto completion* (1)
- *cumbia client* a Qt client to read from and write to sources and targets
- *cumbia ui make* to build cumbia Qt applications
- *cumbia new application* to start writing a project

The *cumbia random* module is a tool to test the behaviour and performance of the upper level modules
(e.g. the graphical components) without connecting to any control system.

(1) Auto completion is provided by the <a href="https://github.com/ELETTRA-SincrotroneTrieste/qumbia-tango-findsrc-plugin">qumbia-tango-findsrc-plugin</a>.

## Library documentation

*cumbia* documentation is hosted on *github pages*.

Visit the dedicated <a href="https://elettra-sincrotronetrieste.github.io/cumbia-libs/">github.io</a> pages.

## Download and installation


### Download the cumbia-libs set of modules

> cd $HOME/Downloads

> git clone https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git

### Check pkgconfig and dependencies

Most of the software relies on *pkgconfig* for its configuration. Please be sure that the
PKG_CONFIG_PATH includes the relevant dependencies required by cumbia. For example:

> echo $PKG_CONFIG_PATH

> /usr/local/lib/pkgconfig:/usr/local/tango-9.2.5a/lib/pkgconfig:/usr/local/omniorb-4.2.1/lib/pkgconfig:/usr/local/zeromq-4.0.7/lib/pkgconfig:/home/giacomo/devel/epics/base-3.16.1/lib/pkgconfig:/usr/local/qwt-6.1.3/lib/pkgconfig

shows the list of the required packages to build all the modules in cumbia-libs: omniorb, tango, zeromq, epics qwt...

### Prepare an ubuntu system for cumbia installation (time: 5 - 8 minutes)

If you are interested in using ubuntu, please refer to the instructions in <a href="README.UBUNTU.md">README.UBUNTU.md</a> file.
The time required to set up a *cumbia*-ready *ubuntu desktop* is about five to eight minutes and the procedure in the README.UBUNTU.md file
requires the installation of official *ubuntu packages* only (by means of *apt-get install* or *Ubuntu Software*).


### Quick installation using the *cubuild.sh* bash script

The cubuild.sh bash scripts installs the cumbia base libraries and apps, including support for either Tango or Epics, or both, if specified.

#### Dependency checklist for an installation with Tango, EPICS and *random* modules

This section can be skipped if you have installed an *ubuntu* system following the instructions in <a href="README.UBUNTU.md">README.UBUNTU.md</a>.

Necessary dependencies

- git
- meson build system + ninja
- tango 9
- qt 5 (with devel packages + designer)
- qwt libraries
- doxygen
- graphviz
- EPICS (v >= 3.14)

Once all dependencies are installed on your system, check the file *scripts/config.sh*, where: 

- PKG_CONFIG_PATH can be set if necessary, uncommenting (and customising) the *pkg_config_path* variable in section I
  (if you prepared an *ubuntu* environment as described in <a href="README.UBUNTU.md">README.UBUNTU.md</a> file
  this is not necessary)
- installation prefix can be changed (section II) if you don't want to install under the default location */usr/local/*

The installation prefix can also be set through an environment variable. We provide two scenarios:

- system wide:

> export install_prefix=/usr/local/cumbia-libs

- locally, e.g. to try the last version:

> export install_prefix=/home/myself/.local/cumbia-libs

EPICS modules require two environment variables:

- EPICS_BASE
- EPICS_HOST_ARCH

The *cumbia-random* doesn't need any special prerequisite.

#### Install *cumbia* through *scripts/cubuild.sh*

The build process can be automated by executing

> scripts/cubuild.sh install tango epics random

from the *cumbia-libs* directory resulting from the *git clone ...* command above.

- To change the installation prefix either edit scripts/config.sh and change *prefix=/usr/local/cumbia-libs* (the default)
  or export the install_prefix environment variable as described above

The command in the example builds and installs the cumbia base modules, cumbia-tango, qumbia-tango-controls,
cumbia-epics, qumbia-epics-controls, cumbia-random, qumbia-apps and qumbia-plugins

Type 

> scripts/cubuild.sh --help for more options

You will be asked some questions during the install process. Read them carefully before answering yes [y] or no [n].
In particular, at the end of the installation, *cubuild.sh* will try to detect if the chosen *prefix* *lib* directory (default: */usr/local/lib*)
is already included in the *ldconfig* search paths. If not, you have two possibilities:

- [y] "yes" answer: add a file cumbia.conf under */etc/ld.so.conf.d/*
- [n] "no", the default: deal with it manually later (either dealing with ldconfig configuration files or exporting a proper LD_LIBRARY_PATH)


If the *quick installation using the cubuild.sh bash script* fails, please read the *step by step installation* procedure to 
point out the possible causes.

#### Post installation
The install procedure will ask your permission to add paths to the sytem *profile*. If this is accepted, you may need to execute
> source /etc/profile
before using cumbia
If you installed the libraries either using a custom prefix or without modifying the system profile, you can set up an
environment in bash by typing
> source $install_prefix/bin/cusetenv.sh

#### Testing
> export TANGO_HOST=test-tango-host:PORT
> cumbia read sys/tg_test/test/double_scalar  sys/tg_test/test/long_scalar


#### Plugins

##### Foreword about plugin path

*cumbia plugins* are installed under

- $install_prefix/lib/qumbia-plugins/

and they are searched under the *QT_PLUGIN_PATH* environment variable at runtime.
The command
> source $install_prefix/bin/cusetenv.sh
exports the variable for you.
Alternatively, the *QT_PLUGIN_PATH* should be correctly set if the *system profile files* installation option was selected.

##### Additional plugin installation

Once installed the main distribution package, you may want to add some plugins.

Change directory one level up:

> cd ..

Clone the desired plugin(s). Example: qumbia-tango-findsrc-plugin for bash auto completion of Tango source names.

> git clone https://github.com/ELETTRA-SincrotroneTrieste/qumbia-tango-findsrc-plugin

> cd qumbia-tango-findsrc-plugin

Follow the build instructions provided with the plugin. Usually, if it is a Qt project, you will need to execute *qmake*, *make* and
*make install*. According to how install_prefix has been chosen in the cumbia-libs installation, two scenarios are possible:

- install_prefix variable in scripts/config.sh untouched *and* no *install_prefix* variable exported before installation:

> qmake

- a custom install_prefix was specified, for example by means of the *export install_prefix* command before scripts/cubuild.sh was called:

> qmake INSTALL_PREFIX=$install_prefix

After qmake, *make* and *make install* can be issued

> make && make install

##### Note
If the install_prefix is the same as that used for *cumbia-libs*, the plugins are installed under

$install_prefix/lib/qumbia-plugins/

and the *QT_PLUGIN_PATH* required to find the plugins for the base *cumbia-libs* can be used for the additional ones as well.
If you proceed otherwise, remember to add the plugin installation directory to *QT_PLUGIN_PATH*.

### Upgrading *cumbia*

If *cumbia* has been installed with the *cubuild.sh* bash script, upgrading is as easy as typing

> cumbia upgrade

on a terminal.

#### Note

If you followed the instructions above, you should have a copy of the *cumbia-libs* sources under *$HOME/Downloads* and executed
*scripts/cubuild.sh* from within *$HOME/Downloads/cumbia-libs*.

The following observations must be taken into account to let *cumbia upgrade* work properly:

- keep the copy of the sources in *$HOME/Downloads/cumbia-libs* to perform periodical upgrades only, not for developing (aka *read-only* copy)
- do not move away that directory.

If you remove the folder, *cumbia upgrade* will not work and you will have to git clone *cumbia-libs* and 
manually execute

> scripts/cubuild.sh update && scripts/cubuild.sh tango install

once again.

## Step by step installation

### The *meson* build system
The [Meson Build System](http://mesonbuild.com) is an open source build system designed
to be fast and user friendly. It provides multi platform support.
The *cumbia*, *cumbia-tango* and *cumbia-epics* modules rely on *meson* for compilation and
installation. Dedicated sections below for each module.
The modules involving the Qt libraries, *cumbia-qtcontrols*, *qumbia-tango-controls*
and *qumbia-epics-controls* adopt Qt's *qmake* build system.
Meson configuration files are named *meson.build*. One file resides inside the main project
directory and another one, intended to build and install the documentation, is under the doc
subdirectory. For the *cumbia* library, you will find:

-  cumbia/meson.build
-  cumbia/doc/meson.build

The *meson.build* files are the sole build configuration files.
They have been prepared by the developer but they can be edited according to your
needs.

#### Requirements

As the [Using Meson](http://mesonbuild.com/Quick-guide.html) guide states,
*meson* has two main dependencies:

-  Python 3
-  Ninja

On Ubuntu these can be easily installed with the following command:

> sudo apt-get install python3 ninja-build meson

Another way (recommended by the guide) to get Meson is to pip install it for your user:

> pip3 install --user meson

#### Notes about compiling a *meson* project

For each *meson* project (*cumbia*, *cumbia-tango* and *cumbia-epics*) you need to create
a separate build directory. For example, to build *cumbia*:

> cd cumbia

> meson builddir

Meson will not allow you to build source code inside your source tree. All build
artifacts are stored in the build directory. This allows you to have multiple build
trees with different configurations at the same time.
This way generated files are not added into revision control by accident.


#### Checking the *meson* project configuration

After *meson buildir* is run, you can *optionally* check the configuration with *meson configure*.
Change into the *build directory*:

> cd builddir

and optionally run:

> meson configure

You can switch on *debug* build:

> meson configure -Dbuildtype=debug

or *release*:

> meson configure -Dbuildtype=release

To change the default prefix (/usr/local and includedir /usr/local/include/cumbia):

> meson configure -Dprefix=/tmp

> meson configure -Dincludedir=/tmp/include

The last command would install the cumbia include files under:

*/tmp/include/cumbia/*

because the instruction: "*install_headers(headers, subdir : 'cumbia')*"
specified by the developer in the *meson.build* file
installs the header files under the *cumbia* child of the include directory.
This observation is valid for *cumbia*, *cumbia-tango* and *cumbia-epics* modules.

Normally, if you don't change the aforementioned options with *meson configure*,
libraries are placed under:

-  */usr/local/lib*

include files under:

-  */usr/local/include/cumbia*
-  */usr/local/include/cumbia-tango*
-  */usr/local/include/cumbia-epics*

and documentation in:

-  */usr/local/share/doc/cumbia*
-  */usr/local/share/doc/cumbia-tango*
-  */usr/local/share/doc/cumbia-epics*

See the [Using Meson](http://mesonbuild.com/Quick-guide.html) documentation for more
details.

### 1. Install cumbia base library

> cd cumbia

Create *build directory* and let meson populate it with the necessary set up files:

> meson builddir

Change into the *build* directory:

> cd builddir

Optionally check the build configuration:

> meson configure

and compile the sources:

> ninja

To recompile after code changes, just type ninja. The build command is always the same.
You can do arbitrary changes to source code and build system files and Meson will detect
them.

To *install* the library, the documentation and the *pkg config* file, run:

> ninja install

To read the *documentation*, execute:

> ninja show-docs

The index.html of the *cumbia* docs should be opened in the default web browser.


### 2. Install the cumbia-tango module if you want to connect to the Tango control system

> cd ../cumbia-tango

> meson builddir

> cd builddir

Optional:
> meson configure

Build library and docs:
> ninja

Install everything:
> ninja install

To read the *documentation*, execute:

> ninja show-docs

The index.html of the *cumbia-tango* docs should be opened in the default web browser.

*Note* if you installed cumbia outside a system wide directory, you may have to
update the PKG_CONFIG_PATH environment so that it contains cumbia's lib/pkgconfig
directory containing the *cumbia.pc* file. For example:

> export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/giacomo/devel/usr/local/lib/pkgconfig

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

The *cumbia-epics* module can be installed as follows:


> cd ../cumbia-epics

> meson builddir

> cd builddir

Configuration through *pkg-config* is not available because some early EPICS versions do not provide a *pkgconfig* file
and newer versions' support seems not complete.
The installation procedure relies on the definition of two variables that can be passed to *meson* with the *-D* option:

> meson configure -Depics_base=$EPICS_BASE
> meson configure -Depics_host_arch=$EPICS_HOST_ARCH

In the example above, EPICS_BASE and EPICS_HOST_ARCH are environment variables defined earlier.
You can provide the actual values to the *-D* options as well:

> meson configure -Depics_base=/usr/local/epics/base-3.14.12.8
> meson configure -Depics_host_arch=$linux-x86_64

Don't forget to specify the installation directory, and the *libdir* option if you want to install the libraries under *lib* rather then *lib64*.
The *-Dbuildtype=* option can be used to build the module in *release* or *debug* mode.
A *meson configure* command condensed in one line looks like this:

> meson configure -Depics_base=$EPICS_BASE -Depics_host_arch=$EPICS_HOST_ARCH -Dprefix=/usr/local/cumbia-libs -Dlibdir=lib -Dbuildtype=release

Build library and docs:
> ninja

Install cumbia-epics:
> ninja install

To read the *documentation*, execute:

> ninja show-docs

The index.html of the *cumbia-epics* docs should be opened in the default web browser.

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

Adjust qt-designer.pro if necessary:

> qmake

> make

> make install


### 8. Build some basic applications

> cd ../qumbia-apps

Some base tools are provided to test the cumbia-libs and perform some basic operations on the
underlying control system (reading or writing quantities).

#### 8a. qumbia-client: a client to read/write from/to Tango and Epics:

> cd qumbia-client

Check qumbia-client.pro and adjust the include directives if needed.
Beware that the lines

> CONFIG+=link_pkgconfig  and PKGCONFIG +=

tell Qt to rely on pkgconfig for dependency resolution. See the comments on the *cumbia-qtcontrols* section above.

To build generic_client type:

> qmake

> make

Read the README.txt for details

> ./qumbia-client test/device/1/double_scalar,giacomo:ai1,test/device/1/double_spectrum_ro

starts the qumbia-client and performs readings on the two Tango attributes specified and the Epics PV giacomo:ai1

> make install

copies the qumbia-client binary file into INSTALL_ROOT/bin. See qumbia-client.pro

If the library has been installed with the *scripts/cubuild.sh* procedure, the command

> cumbia client test/device/1/double_scalar,giacomo:ai1,test/device/1/double_spectrum_ro

will start the client

#### 8b. qumbiaprojectwizard: create a new Qt + cumbia project with a graphical interface.

Change into the project directory:

> cd ../qumbiaprojectwizard

Check INSTALL_ROOT, SHAREDIR, TEMPLATES_INSTALLDIR directives in qumbiaprojectwizard.pro before building the project,
so that make install  places the binary and the templates under $${INSTALL_ROOT}/bin and $${TEMPLATES_INSTALLDIR}/
INCLUDEDIR variable in qumbiaprojectwizard.pro must correctly point to the root include dir where cumbia, cumbia-tango,
qumbia-tango-controls, cumbia-epics, qumbia-epics-controls have been installed, for example */usr/local/*

> qmake

> make

> make install

qumbiaprojectwizard is an *essential tool* to create a project skeleton that is later typically edited with the Qt designer.
You will have to build and install the *cuuimake* tool below in order to successfully build a cumbia Qt application
generated with qumbiaprojectwizard and edited with the Qt designer.

#### 8c. cuuimake: a tool to configure a Qt project with cumbia widgets (parses Qt designer-generated ui files and
expands .h and .cpp files in the project to make their constructors compatible with cumbia-qtcontrols classes
constructors). This is an essential tool.

Go into cuuimake's source directory:

> cd ../cuuimake

Edit cuuimake.pro if you need to adjust the INSTALL_ROOT and the SHAREDIR paths:

> qmake

> make

Install the cuuimake utility that must be used before building any cumbia project created with qumbiaprojectwizard and
edited with the Qt designer:

> make install

Extensive information about cuuimake can be found in the qumbia-tango-controls documentation installed according to the
instructions given in the fourth paragraph. (cuuimake/html/md_src_cuuimake.html)

### 9. Test applications.

#### 9a. Creation and deletion of objects

This application lets you create and destroy objects during execution, to check for memory leaks and
test stability.

> cd qumbia-apps/test/create-delete

> make

Test with an arbitrary number of sources. For example, Tango attributes:

> ./bin/create-delete test/device/1/double_scalar test/device/2/double_scalar test/device/1/long_scalar

#### 9b. Watcher application example

This example shows how to use QuWatcher class to read data and display it on arbitrary objects or internal
class attributes.

> cd ../watcher

> qmake

> make

Launch the watcher application with a Tango device name

> ./bin/watcher   test/device/1


#### 9c. context: an example to understand the *context* (Tango):

This example shows how to send and receive data to/from a reader. The application relies on the Tango engine.
You can start, stop a reader, change the *refresh mode*, the *period*, *get* and *send* data from the link.

> cd qumbia-tango-controls/examples/context

> qmake

> make

Start the application with a Tango attribute as parameter

> ./bin/context test/device/1/double_scalar


# Contacts

[Giacomo Strangolino](mailto:giacomo.strangolino@elettra.eu)




