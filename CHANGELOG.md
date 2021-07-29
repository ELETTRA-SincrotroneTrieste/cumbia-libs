# News
## version 1.3.0

Changes in 1.3.0 shall provide an enhanced performance and brings a
lighter and more readable code in cumbia-tango.

The advance in performance targets not only the native Tango engine:
the *http* module and the interaction with the
[caserver][https://gitlab.elettra.eu/puma/server/canone3] service
will be substantially enhanced.

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

#### CuTReader

CuTReader code has been cleaned and a memory leak regression has been fixed.

#### Applications: qmake options to exclude modules from linking

use *cu_exclude_engines* variable to qmake to list the engines you do not want
to enable in the application.

For example:

```
qmake cu_exclude_engines="websocket epics random"
```

disables support for cumbia-websocket, cumbia-epics and cumbia-random


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
Have a look at [this][https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html]
documentation page.

### cumbia-http
The need to access control systems from home and portable devices
is constantly growing. The new cumbia-http has been designed with
this purpose and http and Server Sent Event services in mind.
Clients of any kind will be able to connect from anywhere through the
[caserver][https://gitlab.elettra.eu/puma/server/canone3] service.

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
More examples [here][https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/qumbia-reader/html/index.html].

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