# cuuimake

### cumbia ui make. A tool to configure a Qt project with cumbia widgets

\b cuuimake is a command line tool that can be found and built under the qumbia-apps/cuuimake/ folder.

### Installation

\li git clone https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs.git
\li cd cqumbia-apps/cuuimake/
\li qmake
\li make
\li make install

The final command will install cuuimake under \a /usr/local/bin/

An xml configuration file describing predefined options and keywords is installed under \a /usr/local/share/cuuimake/cuuimake-cumbia-qtcontrols.xml
This file contains 

\li file name filters to find C++ sources (default: *.cpp, *.h, *.ui, ui_*.h)
\li the name of the available factories (for cumbiapool, cumbiatango and cumbiaepics) and the associated 
\li the name of the classes that must be configured with cumbia. Constructor invocation must be extended with additional parameters.

### cuuimake stages

cuuimake task is accomplished in several stages:

\li \b Clean [optional] make clean is run first and ui_.*.h files are removed, so that they are regenerated and re-expanded.
\li \b Qmake [optional] if the option is enabled [--qmake] qmake is run as a first step
\li \b Analysis of the files in the project directory to find cumbia object declarations
\li \b Expand the files are \a expanded so that the cumbia objects constructor is appropriately invoked with additional parameters
\li \b Uic *Qt uic* is executed on the \a ui to generate the *ui_xxx.h* file.
\li \b Make the make program is run to compile the application [option: --make]

Other cuuimake modes:

\li \b Help the program has been invoked with the --help option
\li \b Info the program is printing informative content

### Personalize cuuimake

Running *cuuimake* with the *--configure* option allows to store some settings permanently in your home directory by means of the Qt QSettings system:

\image html cuuimake-configure.png "cuuimake  --configure"
\image latex cuuimake-configure.eps "cuuimake  --configure"

Reply only with "y" or "n" to the *y|n* questions (without enter) and with a *digit + enter* to the *-jN* option.

*cuuimake --show-config* shows the current configuration

\image html cuuimake-show-config1.png "cuuimake  --show-config"
\image latex cuuimake-show-config1.eps "cuuimake  --show-config"

*cuuimake --show-config* with additional parameters shows the options that <em>would be applied if</em> those options were given:

\image html cuuimake-show-config2.png "cuuimake  --show-config"
\image latex cuuimake-show-config2.eps "cuuimake  --show-config"


### Using cuuimake

#### qmake
Qt qmake must be run before anything else. 

\b Note: *cuuimake --qmake* will run *cuuimake* including qmake as the first stage. *--qmake* option can be permanently enabled through the 
*cuuimake --configure* wizard.

After qmake, *cuuimake* must be run from the project top level directory. Alternatively, run *cuuimake --qmake* or enable qmake option with *cuuimake --configure*.

#### cuuimake

run *cuuimake* after qmake has been successfully executed, from the same (top level) directory where qmake has successfully completed.

This stage loads settings from *cuuimake* configuration files to know what are the classes that need to be expanded, finds the usage of those classes
within the source files and expands them according to either CumbiaTango, CumbiaEpics or CumbiaPool is used. <em>Projects with mixed usage of 
CumbiaTango, CumbiaEpics or CumbiaPool are not supported</em> by *cuuimake*. If *cumbiaappwizard* application has been used to generate the app skeleton, 
the project will be easily analyzed and expanded by cuuimake.


\image html cuuimake.png "cuuimake"
\image latex cuuimake.eps "cuuimake"

As you can see in the last \a analysis line, the detected mode is \a cumbiatango and so the expansion involves adding CuTReaderFactory, CuTWriterFactory and CumbiaTango 
keywords and variable names to the source files. While the user interface file (\a .ui ) does not change or is not removed, cuuimake will not process the source files again.

#### cuuimake with *Qt designer "promoted widgets"*

As shown in the picture below, with Qt designer you can promote a base widget to a custom widget. If the promoted element is a cumbia widget, *cuuimake* must expand its 
constructor as well.

\image html promote.png "Qt designer: promoting a widget"
\image latex promote.eps "Qt designer: promoting a widget"

The promoted class name is QuInputOutput, which is part of the library but at the moment of writing this document is not a plugin yet.

The *ui* file has changed, so we need to run cuuimake once again. If we *make* the project, it wil complain with a message like this:

\image html promote-error.png "make: error with promoted widgets"
\image latex promote-error.eps "make: error with promoted widgets"

That's because the Qt *designer* only generates code where constructors take only a parent QWidget argument. We need to tell *cuuimake* to *expand* the *QuInputOutput* custom
widget. To accomplish this, create a file *cuuimake-quio.conf*. <strong>cuuimake will use all files with a name matching the pattern "cuuimake[.*].conf to apply additional 
expansion to the classes therein listed</strong>.

Edit the *cuuimake-quio.conf* text file and add the following line:

\code
QuInputOutput,cumbiatango,CumbiaTango *,CuTReaderFactory,CuTWriterFactory
\endcode

Comment lines are allowed and start with a '#'. Comma separated fields:

 -# class name, \QuInputOutput
 -# "*engine*", or "*factory*" one of: *cumbiatango*, *cumbiaepics*, *cumbiapool*
 -# list of <strong>qualified</strong> parameter type names that have to be inserted into the cumbia-qtcontrols object constructor
 (*<strong>qualified</strong> class names*, as defined in the main class header file, if the project has been generated with *qumbiaappwizard*)
 
For example, if *qumbiaappwizard* creates a mycumbiatangoapp.h file and a class Mycumbiatangoapp that defines cumbia related attributes like this:

\code
private:
    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;

\endcode

and the custom widget has constructors defined as follows:

\code

    QuInputOutput(QWidget *_parent,  Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, const CuControlsWriterFactoryI &w_fac);
    QuInputOutput(QWidget *parent, CumbiaPool* cu_poo, const CuControlsFactoryPool& f_poo);

\endcode

the *Qt designer* would instantiate QuInputOutput like this:

\code
    widget = new QuInputOutput(Mycumbiatangoapp);
\endcode

within the *src/ui_mycumbiatangoapp.h* file, which would result in an error.

The line in the file *cuuimake-quio.conf*, placed at the top level project directory, alongside the .pro file, tells the *cuuimake* to *expand* the QuInputOutput constructor
inserting the *cu_t, cu_tango_r_fac and cu_tango_w_fac parameters after *Mycumbiatangoapp* (the parent widget):

\code
//         widget = new QuInputOutput(Mycumbiatangoapp); // expanded by cuuimake v.0.1
widget = new QuInputOutput(Mycumbiatangoapp, cu_t, cu_tango_r_fac, cu_tango_w_fac);
\endcode

If *qumbiaappwizard* is used to create an *epics* project, the main class header file contain something like this:

\code
private:
    Ui::Cuepexample *ui;

    CumbiaEpics *cu_e;
    CuEpReaderFactory cu_ep_r_fac;
    CuEpWriterFactory cu_ep_w_fac;
\endcode

and if we promoted a QWidget to a *QuInputOutput*, the lines of the needed *cuuimake-quio.conf* file would have to be like this:

\code
QuInputOutput,cumbiaepics,CumbiaEpics *,CuEpReaderFactory,CuEpWriterFactory
\endcode

#### cuuimake and make 

It is possible to run *cuuimake* immediately followed by *make* choosing one of these options:

\li running *cuuimake --make* 
\li configuring *cuuimake* with *cuuimake --configure* and answering *y* to the "make" question
\li running *cuuimake -jN*, where N is the *make -j* option for pipelined compilation.


#### cleaning ui_.*.h files

*cuuimake --clean*  runs *make clean* and removes all ui_.*.h files under the *ui* directory. (defined in the *cuuimake-cumbia-qtcontrols.xml* global configuration file 
under *<srcdirs><uihdir>*, "ui" by default). This is useful if you want the ui_.*.h files produced by Qt *uic* to be regenerated and re-expanded.
For example, if you modify the <strong>cuuimake.*.conf</strong> file, you need to run *cuuimake --clean* to have the ui_.*.h files refreshed.


