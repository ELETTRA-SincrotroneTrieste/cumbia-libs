# News

## version 2.1.0

Support for Qt6 has been introduced.

Make sure to build qwt libraries with Qt6.
A convenience script named *setenv-qt6.sh* is provided to set up an 
adequate environment for the *qt6* build.
Please inspect the file and *source* it before building for *qt-6*.

Double check

- QWT_ROOT
- CUMBIA_ROOT

so that they match the qwt intallation path and the desired cumbia installation
prefix.

As an alternative, take into account the 
[cumbia-world super project](https://gitlab.elettra.eu/cs/gui/cumbia-world)
described below for a more thorough and simplified installation
procedure.

Mind bug [Generated pkgconfig file for Qt6 requires Qt5 libs](https://sourceforge.net/p/qwt/bugs/353/ ).
Until solved, you need to fix the *Qt6Qwt6.pc* file under *qwt-source/lib/pkgconfig*
Export an updated PKG_CONFIG_PATH, without any reference to older cumbia installation
paths and qt-5, and build with *qmake6*.

### cumbia-world super repository

Under the [Elettra gitlab gui section](https://gitlab.elettra.eu/cs/gui) a 
[cumbia-world](https://gitlab.elettra.eu/cs/gui/cumbia-world) super repository
contains *cumbia-libs* and all the other modules that make part of the 
cumbia world. Please still refer to the 
[github.com ELETTRA-SincrotroneTrieste](https://github.com/ELETTRA-SincrotroneTrieste)
page for the software sources.
The *cumbia-world* project contains a configuration file and a script to build the
desired modules. The modules sources are git *submodules*.

### cumbia

#### CuDType enum alias

The TTT alias for CuDType has been defined:

```
#define TTT CuDType
```

It makes it faster to type the frequently used enumeration name.

#### CuVariant conversion to vector

CuVariant::toVector performance improvements

### cumbia-qtcontrols

New single argument *QWidget* constructors have been made available.
This is aimed at removing *cuuimake* pre processing.
Applications shall compulsorily use *QuApplication*.

## version 2.0.0

The main motivation for a major release is a dramatic improvement in performance.


## Important changes

Please refer to 2.0-CHANGES.md

## version 1.5.1

1. Plot configuration can be saved.

The plot configuration dialog now allows to save changes  to  axes and curve styles.

2. Formula device wildcard replacement

Formulas accept now "$1" wildcards in source names, that are replaced at runtime.

### Performance 

Perf tools have been intensively exploited to find performance issues, especially at application
startup with numerous sources (more than 2000).
Regular expression declarations have become static, so that regex compilation takes place only
once.

#### cumbia

CumbiaPool: if only one cumbia instance has been registered src has no "protocol" prefix, a pointer to that
instance is returned without any regexp matching (since 1.5.1)

#### cumbia-tango

world config has been made static and default src_patterns are added from srcPatterns() 
rather than in CuTangoWorld constructor, which is often called for simple operations

#### cumbia-qtcontrols

CuControlsFactoryPool does not perform regex match if only one factory is registered, simply
returning the sole instance.

Mouse event handler allocates the plugin only at first actual mouse event.

## version 1.5.0

#### cumbia

1. Engine hot switch

Right clicking on any cumbia widget opens a dialog that allows switching from one engine to another.
For example, you can switch from the native *Tango + CORBA* engine to the *http module*.
This is possible, with one caveat: only running sources and targets can be *hot switched*. setSource or
setTargets at runtime at a later moment still have a reference to the original engine, that presently
is not replaced by the procedure.

An application under *quapps/test/engine-switch* is also available for testing.

2. Improved matrix support

Matrices have been improved, fixed and further tested. Their interface and integration with CuVariant has 
evolved to support *serialization* (see below).

Please update the [https://github.com/ELETTRA-SincrotroneTrieste/cumbia-image-plugin](cumbia-image-plugin)
to get support for more data types for images.

3. Data serialization

A new class, *CuDataSerializer*, can encode and decode CuData in order to be sent over the network.
The current implementation assumes both sender and receiver's endiannes is the same. A flag
in the header of the binary message can be checked by the receiver in order to convert the byte order
if necessary.

Data serialization is exploited by the *cumbia http* module in conjunction with the most recent
[https://gitlab.elettra.eu/puma/server/caserver](PUMA caserver) versions (>=1.5.0) that switch to 
binary mode over *Websocket* to transmit a big  amount of data at a higher rate.

##### Documentation

Refer to the CuDataSerializer class documentation for furter information.

4. More flexible data format conversion methods

The *CuVariant* template method *to<T>* now allows a Vector data format to be *to-ed* into a scalar.

#### cumbia-http

The *cumbia-http* module exploits cumbia *data serialization* and binary messages over Websocket
to speed up data transfer for large vectors or images at a high transmission rate. 
This works in conjunction with the versions of [https://gitlab.elettra.eu/puma/server/caserver](PUMA caserver)
starting from 1.5.0.

JSON decoding in cumbia-http supports *matrices*, that are received from the *synchronous PUMA server (caserver-sync)*


#### cumbia-tango

Support Tango 9.4.0 and provide backward compatibility.


#### cumbia-qtcontrols

Fixed *QuInputOutput* misbehaviour when a QComboBox is used as writer in *index mode*.

##### Information dialog redesigned

The *Link stats* information dialog has been completely redesigned to offer a cleaner look
and a leaner user interaction. A tree model is now in use.


##### Support for mouse event plugins

A new *plugin interface* has been introduced so that plugins whose name matches
* *mouse-event-plugin.so* can be loaded in order to process mouse events (press,
release, move, double click).

The [https://github.com/ELETTRA-SincrotroneTrieste/cumbia-mouse-event-plugin](cumbia-mouse-event-plugin)
implements the *drag event* providing a copy action containing the *source or target* 
set on the cumbia-qtcontrols widget.

## version 1.4.0

Refer to [cumbia CHANGES-1.4.md](https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs/blob/master/cumbia/CHANGES-1.4.md)
and [cumbia-tango CHANGES-1.4.md](https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs/blob/master/cumbia-tango/CHANGES-1.4.md)

#### cumbia

Fixed unsigned long long and long long int support.

- Added methods isUnsignedType and isSignedType in CuVariant
- Added *remove* methods to remove keys from CuData. const and non const versions

Important fix:

- after periodic activity timeout changes updates may be lost: fixed timer management
- CuTimerService: when asking for a new timer, always reuse an existing one if the required timeout is the same


### New features

#### cumbia

##### CuData and CuVariant copy on write (implicit sharing)

CuData implements *copy on write*, reducing the internal data copies significantly.

Data is implicitly shared, enabling *copy on write*. Atomic reference counters make the class
reentrant, meaning that methods can be called from multiple threads, but only if each invocation
uses its own data. Atomic reference counting is used to ensure the integrity of the shared data.
Since cumbia 1.4.0, CuData uses implicit sharing as well.


##### Remove from CuData
It is now possible to *remove* keys from CuData. Both const and non const methods are provided.

##### CuUserData

A user defined extension of the CuUserData interface can be used to exchange custom data between 
activities and their clients. This implies passing a pointer instead of a copy of CuData, when 
performance is the highest priority.

### Optimizations

*memory and speed* optimizations characterize release 1.4

#### cumbia

#### cumbia-tango

A new interface named CuTConfigActivityExecutor_I allows customizing attribute and command configuration fetch
from CuTConfigActivity

##### Internal classes and activities optimization

cumbia-tango classes and activities have been optimized for memory and speed, exploiting the *CuData and CuVariant
new implicit sharing*.

##### Update policy on constant polled data

*Poller* can now avoid publishing results when data does not change. This can save a lot of time to applications
reading from several *polled sources*.

Three types of *update policy* are available for *polled sources*:

1. Always update, even though the data does not change (default usual behaviour)
2. Update the timestamp only as long as data does not change;
3. Do not perform any update as long as data does not change. Updates are not sent
   at the *activity* level. This means event posting on the application thread is
   completely skipped, ensuring maximum performance.
   
Changes in error flags, messages or Tango data quality are always notified.

The update policy shall be suggested to the CuPollingService. See also CuPollDataUpdatePolicy.

##### Per thread device factory

*CuTThread* is a CuThread specialization employed in *cumbia-tango* since 1.4.0. Instead of using the shared CuDeviceFactoryService,
which is a global service shared across multiple threads, *CuTThread* owns a dedicated *CuTThreadDevices*, that does not need any
lock to provide shared Tango *DeviceProxy* instances.

#### cumbia-http

The *CuHttpCliIdMan* (client ID manager) has been introduced in order to comply with the new
version and [architecture of caserver](https://gitlab.elettra.eu/puma/server/caserver-proxy) (1.3.5).

The code of the module is now leaner and the CuHTTPActionA, originally base class of other components,
has been removed and the code integrated into CuHTTPChannelReceiver, the only user.

##### Known bug

*nginx* loglevel *info* complains about

``` 
client prematurely closed connection, client: a.b.c.d, server: x.y.eu, request: "GET /sub/chan_name"
```

A [github issue](https://github.com/slact/nchan/issues/638) has been opened in order to find a 
possible fix.

## version 1.3.1

### Fixes

#### cumbia

CuVariant: improved support for UChar and Char types
isInteger returns true if either type is Uchar or Char

#### cumbia-tango

fixes in DEV_UCHAR support

#### cumbia-qtcontrols

Plots: fixed marker positioning when x scale is inverted

### New features

Plots: *QuPlotMarkerComponent*: custom marker contents can be provided by implementing a new interface
called *QuPlotMarkerFormat_I* and defined in quplotmarkercomponent.h, instead of reimplementing 
QuPlotMarkerComponent itself

When building cumbia qt applications, modules can be excluded with the *qmake* option *cu_exclude_modules*.
Since 1.3.0, by default epics,random and websocket modules are excluded.

#### cumbia-qt.prf file

The file *cumbia-qt.prf* in the root directory is used to define global configuration variables for cumbia.
It is installed under the prefix/include directory and included by *quapps.pri*

## version 1.3.0

Changes in 1.3.0 shall provide a better performance and bring a
lighter and more readable code in the cumbia-tango module.

The upgrade targets not only the native Tango engine:
the *http* module and the interaction with the
[caserver](https://gitlab.elettra.eu/puma/server/canone3) service
will be substantially enhanced.

### Fresh plugins

#### cumbia magic

The [cumbia-magic plugin](https://github.com/ELETTRA-SincrotroneTrieste/cumbia-magic-plugin)
transforms any Qt object (QObject, QWidget) into a cumbia reader.
The plugin distribution comes with an example.

#### cumbia image

The [cumbia-image plugin](https://github.com/ELETTRA-SincrotroneTrieste/cumbia-image-plugin)
exploits cumbia 1.3.0 support for *matrices* (see below) to display images on dedicated
widgets. Refer to the [documentation here](https://elettra-sincrotronetrieste.github.io/cumbia-image-plugin/)
and the example included in the source distribution.

## Version changes

### cumbia

#### conversion shortcuts
shortcuts for common conversion methods have been introduced in CuData:
  for example ```d.b("key")``` is the equivalent of ```d["key"].toBool()```
  and ```d.s("src")``` is the equivalent of ```d["src"].toString()```.

The upper case versions: ```d.B(const std::string& key)```,
```d.D(const std::string& key)``` are shortcuts for the template conversion
flavours provided by CuVariant. See CuData documentation.

Matching methods have been introduced in CuVariant.

#### Matrix support

CuMatrix new class is a model for 2 dimensional arrays. See CuMatrix doc. CuVariant constructors have been added for the supported types, given a vector and the number of rows and columns. Template function CuVariant::toMatrix converts a CuVariant matrix to a matrix of type T

#### CuVariant
- vector of char and unsigned char support added

- both CuVariant string and vector of strings conversion now can optionally take a parameter specifying the desired conversion format, printf - style.

#### CuTimer
If an EventLoopService is specified upon timer creation, CuTimer will post
timer events in the event loop service thread (that typically means, the main thread).

### cumbia-tango

#### CuData key simplification

The *keys* used to retrieve information from CuData have become shorter.
For example, *data_type* has become *dt*, *data_format df*, *quality q,
data_format_str dfs*, *state_color sc* and so on.

Reading the [CuData for Tango](https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia-tango/html/cudata_for_tango.html)
is *mandatory* in order to amend code employing the old keys.

This relevant change imposes a review of the code only in the case the developer
has explicitly used the keys to access data in the application. In other words,
simple apps created with the Qt Designer and only built need not be updated.

#### Activities tag

cumbia-tango activities now carry a *tag* in the form of CuData. The *tag*, unlike
configuration options, is carried alongside the results and delivered to the
application.

Factories (CuTangoReaderFactory, CuTangoWriterFactory) can be used to set a *tag*
before *setSource*. The feature is currently used by the
[caserver service](https://gitlab.elettra.eu/puma/server/canone3)

#### Image support

cumbia-tango 1.3.0 introduces *Tango IMAGE* support relying on *cumbia CuMatrix*
for data storage and representation.

#### Message in "msg" key for data without errors

The *msg* key in CuData will be empty if no error message needs to be delivered.
The only relevant information in error-less data is the *timestamp*, that can
always be retrieved through *timestamp_ms* and *timestamp_us* and displayed
at application level.

#### CuTReader

CuTReader code has been cleaned and a memory leak regression has been fixed.

#### Developer notes

- CuActionFactoryService moved to a *impl* pattern. CuActionFactoryServiceImpl
  is the only implementation.
- CuActionFactoryService.findActive becomes CuActionFactoryService.find, because
  code refactoring simplified the whole implementation.

### cumbia-qtcontrols

The module adapts to the lower level library changes just described.

QuLabel border drawing has been made less intrusive when the *internal border*
is disabled.

If min and max values are not available in the configuration phase yet format is,
configure writers with format only and do not impose a min max other than the one
suggested by the format (ELETTRA request #13695).
See also *qumbia-tango-controls/examples/writers_format_test* example.

CuControlsReader sendData now accepts an *args* key to update input arguments for
writings *at runtime* (typically, Tango commands). See *qumbia-tango-controls/examples/editargs*.

### qumbia-tango-controls

#### editargs example

Please see *qumbia-tango-controls/examples/editargs* example in order to
see how arguments for *writers* (typically, Tango commands) are treated
and changed at runtime.

### Applications

#### qmake options to exclude modules from linking

use *cu_exclude_engines* variable to qmake to list the engines you do not want
to enable in the application.

For example:

```
qmake cu_exclude_engines="websocket epics random"
```

disables support for cumbia-websocket, cumbia-epics and cumbia-random

#### cumbia read: option to disable configuration

An option (-n, --no-properties) disables property fetch at startup (typically:
Tango database configuration)

#### cumbia read: matrix support

Try to read a Tango *IMAGE*!

> cumbia read -n test/device/1/double_image_ro

(-n: use the *native* engine: *http* module / [caserver](https://gitlab.elettra.eu/puma/server/canone3)
may not support matrices yet).

## version 1.2.4

### cumbia-tango

CuTConfigActivity: if option "no-value" is found and set to true, then neither command_inout nor read_attribute are performed during configuration. Fixed to conversion in CuTConfigActivity that may prevent proper readers configuration Removed "action_ptr" information in polling related CuData. CuPollingActivity action_data internal map stores std::string --> ActionData instead of ptr

### cumbia-http

CuHttpControlsWriter.setTarget and CuHttpControlsReader.setSource set option "read-value" to true on CuHTTPActionConfFactory when it is safe to execute command_inout during configuration. Fixes a bug that caused command_inout to be imparted during writer configuration. In principle, "read-value" set to true should apply only to commands, and should not be necessary when dealing with attributes. This must be dealt on the server side. When this is fixed, then the property could be "do-cmd-read" or something else to have a proper meaning

### cumbia-qtcontrols
NumberDelegate/DecIntFromFormat: without "min" and "max" properties in the configuration stage, the "format" is used to configure writers (applies to quinputoutput, quapplynumeric) QuWatcher: added convenience class Qu1TWatcher, one time, auto destroying QuWatcher

### qumbia-tango-controls
under examples, a new test application named "writers_format_test" to test cumbia-qtcontrols numeric widgets configuration with format and without min and max properties


## version 1.2.3

### Introduction

Version 1.2.3 ameliorates engine transparency introducing a special syntax for
sources to fetch Tango database properties.
Numerous small issues have been fixed and a new feature for the Tango (and the http)
engines allows changing source arguments in (polled) commands on the fly through a
lightweight call to CuContext::sendData.

### cumbia-tango

- Tango database read support integrated in *setSource*

- sendData supports the "args" key to change the source arguments while reading (Tango, http)
getContext()->sendData() supports an "args" parameter to change the source arguments for readers

#### Example

Please see
```qumbia-tango-controls/examples/editargs```

## version 1.2.0

### Description
The cumbia version 1.2.0 moves definitely towards the application portability
and aims at broader integration across several engines (that translates into
cumbia modules).
Alongside the *websocket* integration, *cumbia 1.2.0 introduces initial support to
http and Server Sent Events technologies*.

Please see:

https://gitlab.elettra.eu/puma/server/canone3

Applications written in any language on any platform will thus be enabled to connect to native
control system engines from anywhere.

#### quapps.pri project include file

The qumbia-apps module includes a new quapps utility that offers automatic
cumbia module (engine) detection at application startup.

Just include quapps.pri from your path/to/cumbia-libs/include/quapps to use the
CuModuleLoader helper class.

Tango Database read access becomes easy!
Reading class properties is accomplished by a source like tango://TangoTest(ProjectTitle,Description) Device properties can be obtained like tango://ken:20000/test/device/1(description, helperApplication,values), and get the list of attributes of a device through ken:20000/test/device/1(description, helperApplication,values)

Type

```
cumbia read --help
```

and scroll to the section Read the Tango database to play and get acquainted
with this new functionality in no time!

This new feature allows to get properties in your app with just the little
code needed for usual sources, and integrate them easily in graphical elements.
Have a look at [this](https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html)
documentation page.

### cumbia-http
The need to access control systems from home and portable devices
is constantly growing. The new cumbia-http has been designed with
this purpose and http and Server Sent Event services in mind.
Clients of any kind will be able to connect from anywhere through the
[caserver](https://gitlab.elettra.eu/puma/server/canone3) service.

#### Build cumbia-libs with http support

Include cumbia-http in your installation is as easy as adding http to
the scripts/cubuild.sh command.

### CuModuleLoader in applications

The new header file provided by *qumbia-apps/quapps* module allows to
find and load all available cumbia engines, depending on optional command line arguments.
Thanks to this and to the flexible cumbia design, it is possible to connect any
application to a native control system engine or let it obtain data through
*http* or *websocket*.

#### QTango migration
QTango migration creates a multi engine version of the app.

The cumbia project wizard now creates a multi engine version of the application
when migrating from a QTango project.

## version 1.1.1
### cumbia
- CumbiaPool: removed printf when no implementation is found and improved doc
- Support for cross compilation with emscripten / wasm
### cumbia-qtcontrols
- CuControlsFactoryPool: NULL replaced with nullptr.
- A QuReplaceWildcards_I definition has been introduced to provide an interface for command line *replace wildcards* capable engines.

### qumbia-apps

- qumbia-read: support for websocket

### qumbiaprojectwizard:
- fixed $APPLICATION_NAME$" and "$ORGANIZATION_NAME$" placeholder replacement
- multi-engine is the default option and the templates create a skeleton including cumbia-random and cumbia-websocket modules, if available. cumbia-websocket is added only if a proper command line option is detected. QCommandLineParser is used to find the required option to activate cumbia-websocket connection

### cumbia-websocket
- added support for a full-duplex communication with generic server on a single web socket. Former support to canoned (pwma) server is maintained and selected if an http url is provided to the constructor alongside the websocket URL.
d1. can be tested using the simple https://github.com/ELETTRA-SincrotroneTrieste/cumbia-websocket-proxy-server.git websocket server "proxy"
added support for command line argument wildcard replacement for tango by an implementation of QuReplaceWildcards_I

### qumbia-tango-controls

- cutcontrols-utils: CuTangoReplaceWildcards implementation of the QuReplaceWildcards_I interface is used to perform command line arguments wildcard replacement

## version 1.1.0
App: cumbia read
cumbia read has been expanded in order to support the cumbia-random module
and a great number of command line options.
Coupled with the qumbia-tango-findsrc-plugin and the qumbia-tango-find-src app,
that can be installed from github. it provides Tango sources auto completion on
the command line.

Try it out!
Hint: hit the TAB key to auto complete Tango source names.

description	command
read sources from heterogeneous engines



> cumbia read test/device/1/double_scalar epics:ai1 random://rnd/1/0/10


read sources configuration only, from heterogeneous engines

> cumbia read test/device/1/double_scalar epics:ai1 --property

Impart a Tango command to read an array three times, truncate the output to 8 elements,
print with a custom format

> cumbia read inj/diagnostics/rtbpm_inj.01//GetHorPos[0,1000] --truncate=8 --3 --format="%.4f"

Monitor a Tango command

> cumbia monitor inj/diagnostics/rtbpm_inj.01//GetHorPos[0,1000] --truncate=8

Filtered list of Tango device properties	cumbia read --tp test/device/1:helper*
Read a Tango attribute property	cumbia read --tp test/device/1/string_scalar/values
More examples [here](https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/qumbia-reader/html/index.html).

Additional features:

customizable output detail level
support for Tango database properties (device, attribute, class)
either monitor or single shot mode
See the documentation for a more detailed description.
Library
New prerequisites
The read/write locks adopted to provide greater speed require a compiler
supporting the C 17 standard.

### cumbia
Version 1.1.0 allows the application developer to customize how threads are
grouped together, as well as to define an upper limit on their number.
The *cumbia-tango* engine groups threads by Tango device.
If you want to customize the default behaviour, refer to the CuTThreadTokenGen
and the Cumbia::setThreadTokenGenerator documentation pages.

Additionally, timers in polled reads can be reused in order to limit timer threads.
See the CuTimerService documentation.

### cumbia-tango
#### Database search
CuTdbPropertyReader

- added support for wildcards when searching device properties
- added Tango class property list for a given class name

### cumbia-random
cumbia-random has been expanded in order to offer great flexibility to test
higher level cumbia modules and applications with greater accuracy.
Source names alone can specify how data is generated, threads grouped and much more.
JavaScript functions in .js files can be set as source of data for the cumbia-random
module.

The source name can define group threading, period, size of generated data, minimum,
maximum and period.
The source can specify a JavaScript file to generate data from a custom function
cumbia-qtcontrols

#### CuPluginLoader

The CuPluginLoader provides a template method to quickly obtain an instance
of the desired plugin. See CuPluginLoader::get.

### Plugins
#### qumbia-tango-findsrc-plugin
An external new plugin named qumbia-tango-findsrc-plugin and the
qumbia-tango-find-src app are available from github.com .
The utility finds the name of a Tango source (attribute or command)
from a partial name. The plugin can be loaded from any application willing to
provide Tango sources auto completion.
