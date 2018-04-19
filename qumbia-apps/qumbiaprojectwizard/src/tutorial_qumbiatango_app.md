# Setting up a graphical application with cumbia and tango. {#tutorial_qumbiatango}

This tutorial explains how to set up a cumbia graphical user interface with cumbia-qtcontrols widgets connected to the 
tango control system.

#### Requirements

The following components must be installed:

\li cumbia
\li cumbia-tango
\li cumbia-qtcontrols
\li qumbia-tango-controls
\li qumbia-plugins/qt-designer
\li qumbia-apps/cuuimake, the tool to process ui, pro, h and cpp files and
\li qumbia-apps/qumbiaprojectwizard, the tool to create a new cumbia project

#### Install qumbiaprojectwizard

Change directory into *qumbia-apps/qumbiaprojectwizard* and execute

\code
qmake
make -j5
sudo make install
\endcode

To build all the *qumbia-apps* utilities, the same code can be run from the *qumbia-apps* parent directory.

#### Command line

\code
cumbia new project 
\endcode

### Create a new qumbia tango application

Start the qumbiaprojectwizard application.
\n Fill in all the relevant fields as shown in the screenshot below. In the \b Support box on the top right, select "tango", and choose a project name.
If you want, you can automatically open the \b qt-designer and \b qtcreator applications when 
qumbiaprojectwizard is closed. 

\image html qumbiaprojectwizard.png "qumbiaprojectwizard application"
\image latex qumbiaprojectwizard.eps "qumbiaprojectwizard application"

### Create the UI with qt designer
If not launched by the qumbiaprojectwizard app, start the qt designer and edit the graphical interface. Usual Qt widgets can be used together with 
\b cumbia-qtcontrols graphical elements.

The screenshot below shows a simple widget with a \a QuLabel, a \a QuLed and a \a QuTrendPlot.

\image html designer.png "Qt designer with cumbia-qtcontrols widgets"
\image latex designer.eps "Qt designer with cumbia-qtcontrols widgets"

Up to this point, the process of creating a cumbia Qt application is completely the same as QTango's. The syntax for sources and targets is the same:

\li tango/device/name/att_name for sources and targets
\li tango/device/name->command for sources and targets with an empty input argument 
\li tango/device/name->command(10,100,..) for sources and targets with two or more input arguments
\li tango/device/name->command(&otherObject_name)  for sources and targets with input argument to be taken from other widgets in the application
\li tango/device/name->command(12,100,&objname1,&objname2,-25,...,&objNameN)  for sources and targets with mixed input arguments, constant and taken from other widgets.

Once the \a ui file is saved, you have a very basic application made up of a main.cpp, a ui file, a .cpp and a .h file containing the main class definition and implementation.
The application is able to read / write the sources and targets associated to each cumbia-qtcontrols widget. In QTango, you would run:

\li qmake
\li make

to build the project.

To build a cumbia-qtcontrols project, you need the \a cuuimake command line utility. It parametrizes all cumbia-qtcontrols widget constructors as required.
Please read the \ref md_src_cuuimake documentation before proceeding to the next section.

In cumbia, to build a project you would run

\li qmake
\li cuuimake
\li make

or, alternatively

\li cuuimake --qmake --make

in a single line.

If the project includes custom *cumbia "promoted widgets"*  in the *ui* drawn with the *Qt designer*, a file with a name like <em>cuuimake.*.conf</em> must be created in order to let
*cuuimake* properly deal with class constructors expansion. See the \ref md_src_cuuimake documentation for more details.






