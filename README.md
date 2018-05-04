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

> cd $HOME/Downloads

> git clone https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git

### Check pkgconfig and dependencies

Most of the software relies on *pkgconfig* for its configuration. Please be sure that the
PKG_CONFIG_PATH includes the relevant dependencies required by cumbia. For example:

> echo $PKG_CONFIG_PATH

> /usr/local/lib/pkgconfig:/usr/local/tango-9.2.5a/lib/pkgconfig:/usr/local/omniorb-4.2.1/lib/pkgconfig:/usr/local/zeromq-4.0.7/lib/pkgconfig:/home/giacomo/devel/epics/base-3.16.1/lib/pkgconfig:/usr/local/qwt-6.1.3/lib/pkgconfig

shows the list of the required packages to build all the modules in cumbia-libs: omniorb, tango, zeromq, epics qwt...

### Quick installation using the *cubuild.sh* bash script

The cubuild.sh bash scripts installs the cumbia base libraries and apps, including support for either Tango or Epics, or both, if specified

#### Dependency checklist for an installation with Tango modules

- qt 5 (with devel packages)
- meson build system
- tango 9
- qwt libraries

Once all dependencies are installed on your system, the build process can be automated by executing

> scripts/cubuild.sh install tango 

from the *cumbia-libs* directory resulting from the *git clone ...* command above.

The command in the example builds and installs the cumbia base modules, cumbia-tango, qumbia-tango-controls, qumbia-apps and qumbia-plugins

Type 

> scripts/cubuild.sh --help for more options

If the *quick installation using the cubuild.sh bash script* doesn't work for you, please read the *step by step installation* procedure to 
point out the possible causes.

### Upgrading *cumbia*

If *cumbia* has been installed with the *cubuild.sh* bash script, upgrading is as easy as typing

> cumbia upgrade

on a terminal.

If you followed the instructions above, you should have a copy of the *cumbia-libs* sources under *$HOME/Downloads* and executed
*scripts/cubuild.sh* from within *$HOME/Downloads/cumbia-libs*.

The following observations must be taken into account to let *cumbia upgrade* work properly:

- keep the copy of the sources in *$HOME/Downloads/cumbia-libs* to perform periodical upgrades only, not for developing;
- do not move away that directory.

If you move away the folder, *cumbia upgrade* will not work and you will have to git clone *cumbia-libs* and 
manually execute

> scripts/cubuild.sh update && scripts/cubuild.sh tango install

again.

## Step by step installation

### The *meson* build system
The [Meson Build System](http://mesonbuild.com) is an open source build system designed
to be fast and user friendly. It provides multi platform support.
The *cumbia*, *cumbia-tango* and *cumbia-epics* modules rely on *meson* for compilation and
installation. The modules involving the Qt libraries, *cumbia-qtcontrols*, *qumbia-tango-controls*
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

> sudo apt-get install python3 ninja-build

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

If you rely on the Epics control system, please install this module as follows:

> cd ../cumbia-epics

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

#### 8a. generic_client: a client to read/write from/to Tango and Epics:

> cd generic_client

Check generic_client.pro and adjust the include directives if needed.
Beware that the lines

> CONFIG+=link_pkgconfig  and PKGCONFIG +=

tell Qt to rely on pkgconfig for dependency resolution. See the comments on the *cumbia-qtcontrols* section above.

To build generic_client type:

> qmake

> make

Read the README.txt for details

> ./generic_client test/device/1/double_scalar,giacomo:ai1,test/device/1/double_spectrum_ro

starts the generic_client and performs readings on the two Tango attributes specified and the Epics PV giacomo:ai1

> make install copies the generic_client binary file into INSTALL_ROOT/bin. See generic_client.pro

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




