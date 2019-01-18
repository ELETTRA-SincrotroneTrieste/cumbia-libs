# Add cumbia to a fresh ubuntu installation

In this document we will describe the necessary steps to set up *cumbia* in a freshly installed *ubuntu 18.04* desktop edition.

We downloaded and installed an *ubuntu desktop 18.04* from the iso image found at:

https://www.ubuntu.com/download/desktop/contribute?version=18.04&architecture=amd64

#### Note
The *Normal installation* default option has been chosen during the installation procedure
(under the step *Updates and other software* / *What apps would you like to install to start with?* )

## Dependencies

### Compiler, make, meson build system, git, doxygen, ... 

The following command installs the GNU compiler, GNU make, meson...

> sudo apt-get install  build-essential  ninja-build  meson  git  doxygen graphviz

### Tango libraries

> sudo apt-get install libtango-dev libtango-tools tango-test

The command installs tango version 9.2.5a and the *TangoTest* server (*/usr/lib/tango/TangoTest*)

#### Setting TANGO_HOST
During the installation, the *Configuring tango-common* dialog will ask you to specify the name of the host where the *TANGO* database server is running.
Type the *hostname:port* couple and continue.

### Qt 5 libraries

Install the qt5 libraries for graphical applications, the *qt designer* and the *qt creator* ide

> sudo apt-get install qt5-default qtcreator libqt5x11extras5-dev qttools5-dev 

Qt version *5.9.5* and *qtcreator* version *4.5.2* will be installed.

- libqt5x11extras5-dev provides the QT module *x11extras*, needed by the qumbia-plugins/qt-designer
- qttools5-dev provides the QT module *designer*, needed by the qumbia-plugins/qt-designer

In order to build the qml module, the following installation is necessary

> sudo apt-get install qml-module-qtcharts qml-module-qtquick-controls2 qml-module-qtquick-dialogs qml-module-qtquick-extras qml-module-qtquick-scene2d qml-module-qtquick-scene3d qml-module-qtquick-templates2  qtdeclarative5-dev libqt5charts5-dev qtcharts5-examples qtcharts5-doc-html 

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


### cumbia libraries

Download *cumbia* sources to a place (e.g. ~/Downloads) where they will be preserved for future upgrades (no development in there!):

> git clone  https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git

Change into the *cumbia-libs* directory and execute:

> ./scripts/cubuild.sh  tango install 

This will build and install the *cumbia* base libraries, the *Tango* modules (*cumbia-tango* and *qumbia-tango-controls*) and some 
utilities ("<em>cumbia apps</em>").

Please read <a href="README.md#install-cumbia-through-scriptscubuildsh">Install cumbia through scripts/cubuild.sh</a> 
in <a href="README.md">README.md</a> for more details.






