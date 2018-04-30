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

















