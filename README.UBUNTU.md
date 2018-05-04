# Add cumbia to a fresh ubuntu installation

In this document we will describe the necessary steps to set up *cumbia* in a freshly installed *ubuntu 18.04* desktop edition.

#### Note
The *ubuntu minimal* option was chosen during the installation procedure.

## Dependencies

### Compiler, make, meson build system, git, doxygen, ... 

The following command installs the GNU compiler, GNU make, meson...

> sudo apt-get install  build-essential  ninja-build  meson  git  doxygen graphviz

### Tango libraries

> sudo apt-get install libtango-dev libtango-tools tango-test

The command installs tango version 9.2.5a and the *TangoTest* server (*/usr/lib/tango/TangoTest*)

### Qt 5 libraries

Install the qt5 libraries for graphical applications, the *qt designer* and the *qt creator* ide

> sudo apt-get install qt5-default qtcreator

Qt version *5.9.5* and *qtcreator* version *4.5.2* will be installed.


### Qwt libraries: widgets for technical applications

The Qwt libraries are used by *cumbia-qtcontrols* to provide *plot* widgets. Version 6 for *qt5* must be installed:

> sudo apt-get install libqwt-qt5-dev

The additional packages libqwt-headers and libqwt-qt5-6 are installed.

#### Observations 

About *ubuntu Qwt* installation:

- Qwt include files are placed under /usr/include/qwt
- Qwt libraries are named libqwt-qt5.so
- No pkg-config file is installed

*cumbia* relies on *pkg-config* to find dependencies. Nevertheless *Qwt* installation on ubuntu should be detected.




