# Understanding the cumbia-qtcontrols widgets constructors, sources and targets. {#understanding_cumbia_qtcontrols_constructors}

cumbia-qtcontrols widgets have been designed to be parametrized at construction time so that read and write operations can be
performed through several different engines. Thanks to a common interface and to the help of engine specific factories, a
widget can connect to either one or another engine transparently.

In this chapter, we will discuss the two main constructors you will always find in the cumbia-qtcontrols widget library.

## Readers

### Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.

Taking QuLabel as an example, the constructor is declared as follows:

> QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

\li w is the parent widget, as usual for every Qt widget;
\li cumbia is a pointer to a Cumbia implementation specific to a given engine, for example CumbiaTango or CumbiaEpics
\li r_fac is a const reference to an implementation of a CuControlsReaderFactoryI interface, such as CuTReaderFactory
from the qumbia-tango-controls module.


### Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*.

*CumbiaPool*  and *CuControlsFactoryPool* are two objects that allow mixing together in the same application different engines.

Taking again QuLabel as an example, the constructor looks like this:

> QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

\li w is the parent widget;
\li cumbia_pool is a pointer to an object designed to configure the QuLabel at runtime according to the source provided 
for the reader. If CumbiaPool parametrizes QuLabel, Tango and Epics sources can be used together in the same application. 
\li fpool: a const reference to a factory that allocates the appropriate reader depending on the source specified.


### Sources

A source in *cumbia* is the name (address) of a quantity that can be read.

The name of the source depends on the engine employed, and identifies a
connection endpoint, e.g. for Tango a device or server name plus the name of a
command or attribute, and for Epics a PV. A couple of examples follow:

\li *test/device/1/double_scalar*: an example of Tango attribute that will be read.
\li *controls:current*: an epics PV to read from
\li *a/b/c->GetValues(0, 100)*: reads a Tango command that receives two input arguments.
\li *$1/attribute_name*: replaces $1 with the first command line argument of the application, which will be a Tango device name. The attribute *attribute_name* will be read.
\li *$2->command_name*: replaces $2  with the second command line argument of the application, which will be a Tango device name. The command *command_name* will be read.

#### Sources to access the Tango database

In this section we cover source names that allow to fetch properties and other pieces of information from
the Tango Database. The *cumbia read* utility used in the examples allows immediate testing.

\li    Domain search: src must end with wildcard:  tango://host/port/doma*
```bash
        $ cumbia read  tango://ken:20000/seq*
        $ * tango://ken:20000/seq*: [16:10:42+0.569857] [string,2] { seq,sequencer }
```

\li    Search all families under a given domain:
```bash
        $ cumbia read tango://ken:20000/sequencer/
        $ * tango://ken:20000/sequencer/: [16:27:33+0.470669] [string,3] { fermi,ps,test }
 ```
\li    Search all families under a given domain, with wildcard:
```bash
        $ cumbia read tango://ken:20000/sequencer/fe*
        $ * tango://ken:20000/sequencer/fe*: [16:29:02+0.798002] [string,1] { fermi }
```
\li    Search all members under a given domain/family
```bash
        $ cumbia read tango://ken:20000/sequencer/fermi/
        $ * tango://ken:20000/sequencer/fermi/: [16:30:11+0.886962] [string,2] { test.01,test.02 }
```
\li    Search all members under a given domain/family, with wildcard:
```bash
        $ cumbia read tango://ken:20000/sequencer/test/s*a*
        $ * tango://ken:20000/sequencer/test/s*a*: [16:31:42+0.568593] [string,3] { scan1d-test.01,stats-01,stats-02 }
```
\li    Get a free property from an object in the database
```bash
        $ cumbia read tango://ken:20000/#Sequencer#TestList
        $ * tango://ken:20000/#Sequencer#TestList: [16:40:17+0.056525] [string,3] { seq/test/launch_super,seq/test/launch_a,seq/test/launch_b }
```
\li    Get the list of attributes of a device
```bash
        $ cumbia read tango://ken:20000/test/device/1/
        $ * tango://ken:20000/test/device/1/: [16:44:02+0.281213] [string,62] { ampli,boolean_scalar,double_scalar,double_scalar_rww,double_scalar_w,...
```
\li    Get device properties
```bash
        $ cumbia read "tango://ken:20000/test/device/1(description, helperApplication,values)"
        $  * tango://ken:20000/test/device/1(description, helperApplication,values) [11:32:55+0.426527]
           - tango://ken:20000/test/device/1
             description               --> This is a Tango Test device with ,many interesing attributes and commands
             helperApplication         --> atkpanel
             values                    --> 10,20,30,40,50
```
\li    Get one attribute property
```bash
        $ cumbia read cumbia read "tango://hokuto:20000/test/device/1/double_scalar(values)"
        $ * tango://hokuto:20000/test/device/1/double_scalar(values) [11:35:45+0.346103]
          - tango://hokuto:20000/test/device/1
            values                    --> 10.1,10.2,10.3,12.0,16.2,80.12,100.1,200.8
```
\li    Get multiple attribute properties
```bash
        $ cumbia read "tango://hokuto:20000/test/device/1/double_scalar(min_value,max_value,min_alarm,max_alarm,format)"
        $ * tango://hokuto:20000/test/device/1/double_scalar(min_value,max_value,min_alarm,max_alarm,format) [11:43:02+0.779032]
          - tango://hokuto:20000/test/device/1
            format                    --> %2.3f
            max_alarm                 --> 200
            max_value                 --> 255
            min_alarm                 --> -200
            min_value                 --> -255
         -------------------------------------------------
```
\li    List all properties of a given class
```bash
        $ cumbia read "tango://hokuto:20000/TangoTest(*)"
        * tango://hokuto:20000/TangoTest(*) [12:02:44+0.146309]
          -
          - class: TangoTest
          - pattern: "TangoTest(*)"
          - value: Csoglia,cvs_location,cvs_tag,Description,doc_url,helperApplication_old,InheritedFrom,ProjectTitle
          - tango host: hokuto:20000
```
\li    Fetch class properties
```bash
        $ cumbia read "tango://hokuto:20000/TangoTest(ProjectTitle,Description)"
        $ * tango://hokuto:20000/TangoTest(ProjectTitle,Description) [12:08:04+0.329729]
          -
            ProjectTitle              --> TANGO Device Server for testing generic clients
            Description               --> A device to test generic clients. It offers a "echo" like command for each TANGO data type (i.e. each command returns an exact copy of <argin>).
```


## Writers

There exists a perfect analogy between the readers and writers parametrization. Taking a QuWriter as example, the two constructors are as follows:

> QuWriter(QObject *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);

> QuWriter(QObject *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

The only difference can be seen in the first constructor, where we find a CuControlsWriterFactoryI const reference, the 
analogy with the reader's CuControlsReaderFactoryI is clear.


### Targets

A target in *cumbia* is the name (address) of a quantity that can be written, i.e. it's the 
*target* of an action.

Methods:

> void setTarget(const QString& target);

> QString target() const;

The name of the target depends on the engine employed, and identifies a
connection endpoint, e.g. for Tango a device or server name plus the name of a
command or attribute, and for Epics a PV. A couple of examples follow:

\li *test/device/1/double_scalar*: an example of Tango attribute;
\li *b/power_supply/psb_s1.1->SetCurrent(10)*: calls the SetCurrent command to a Tango device with 10 as input argument
\li *test/device/2/long_scalar(&doubleSpinBox)* writes the value taken from a QDoubleSpinBox named "doubleSpinBox" into the double_scalar attribute of test/device/2
\li *$1/long_scalar(&doubleSpinBox)* writes the value taken from a QDoubleSpinBox named "doubleSpinBox" into the double_scalar attribute of the device specified as *first*
command line argument of the application.
\li *control:ai1* is an example of *epics* target 


Independent of the syntax of the names, an expression can be used to provide one or more input arguments
to the command. The input argument(s) can be:

\li provided statically: *test/device/one->command(10,100)*;
\li taken from *another* editable widget: *a/b/c->command(&pressureSpinBox)*.
Here *pressureSpinBox* is the object name of a QDoubleSpinBox used to give a
double value as input for the command *a/b/c->command*

In the second case, the editable widget providing the input must implement one of the
following properties:

\li text (example: a QLineEdit)
\li value (examples: QSpinBox, QDoubleSpinBox, QuApplyNumeric, QProgressBar, ...)
\li currentText (example: QComboBox).

Custom widget that provide input arguments for targets can be implemented, as long as
they provide one of the three properties above mentioned.
The properties listed above are evaluated by the CuControlsUtils::findInput in that
exact order.

Users of QTango should recognize the equivalent syntax.
A relevant difference is that in *cumbia* you normally set only one target on a widget.
The method is actually called *setTarget*, not *setTargets* as it was in QTango. Unlike QTango,
it is enough for the widgets providing input arguments to have one of the three listed properties.

## The *qumbiaprojectwizard* utility, the *Qt designer* and the *cuuimake* tool

The *qumbiaprojectwizard* application, the Qt designer and the *cuuimake* tool from qumbia-apps, make developing 
cumbia applications very easy. see the <a href="../../qumbia-tango-controls/html/md_src_tutorial_qumbiatango_widget.html">
writing a qt widget that integrates with cumbia</a> tutorial for an example.

















