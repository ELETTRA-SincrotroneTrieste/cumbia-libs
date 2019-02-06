# Frequently Asked Questions (Tango) {#tutorial_faq}



### Table of contents
- [Is there a way to quickly set up a new cumbia project?](#newproj)
- [Is there a way to migrate a QTango project into a cumbia one?](#migrate)
- [What is the default structure used to exchange data by cumbia library? How do I use it?](#cudata)
- [What is the quickest way to read a Tango attribute in a cumbia application](#readatt)
- [What's the quickest procedure to display a Tango *state* and possibly get the associated color?](#state)
- [How did you know that the *res* CuData contained those very keys such as "value", "state_color", "timestamp_ms", and so on.. ?](#cudata_keys)
- [A quick way to perform a command inout on a device](#commands)
- [In QTango there used to be a widget ready to read and display a value. In cumbia there is not. How do I quickly adapt an existing Qt widget?](#cumbiawidget)
- [I either used QuWatcher or implemented CuDataListener on my custom graphical object. How do I configure it through the Tango database properties (setting maximum and minimum values, display unit and data format)?](#configure)
- [How do I get a Tango device property?](#tangoprops)
- [How to format a message from a Tango *Exception*?](#except)
- [How to support multiple engines (e.g. Tango and Epics) in the same application](#multiengine)
- [](#)

## Q.

<a name="newproj"/>
### Is there a way to quickly set up a new cumbia project?

## A.
Yes, just execute

*cumbia new project*

from the command line. You will

## Q.

<a name="migrate"/>
### Is there a way to migrate a QTango project into a cumbia one?

## A.
Yes, execute

*cumbia import fast*

from the command line to *attempt* the migration. Follow the instructions and carefully
read the messages from the ongoing process. A little manual intervention is normally needed.
It works for simple QTango projects (e.g. custom QTangoComProxyReader/QTangoComProxyWriter
derived classes are not supported)



## Q.
<a name="cudata"/>
### What is the default structure used to exchange data by cumbia library? How do I use it?

## A.

It is a class named CuData. It is a bundle pairing *keys* to *values*.
*keys* are strings, while values are CuVariant objects. CuVariant is a container that
can store different data types and provides convenient functions to extract data into
basic types, such as double, int, unsigned, vector<double>, vector<string> and so on.

Example usage
```cpp
void f(const CuData& data) {
    double d;
    CuVariant v = data["value"];
    d = v.toDouble();
}
```

## Q.
<a name="readatt"/>
### What is the quickest way to read a Tango attribute in a cumbia application:
 - blocking for the result
 - storing the result in a cumbia data structure that can be reused throughout the library
 - extracting the result/errors

## A.

```cpp
#include <cutango-world.h>
#include <tdevice.h>
#include <QDateTime>

int main(int argc, char *argv[])
{
    if(argc < 3) {
        printf("usage: %s tango/test/device  attribute_name\n\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    CuTangoWorld tw; // Tango utilities to read/write attributes and command inout
    CuData res; // stores the result
    TDevice td(argv[1]); // wraps DeviceProxy creation and device connection errors
    bool success = td.isValid();  // device defined in Tango database
    if(!success)
        printf("failed to connect to device: %s: %s\n", argv[1], td.getMessage().c_str());
    else {
        success = tw.read_att(td.getDevice(), argv[2], res);  // read attribute, store result in res, return success
        if(success) {
            double d = res["value"].toDouble(); // extract to double (I know beforehand I am reading a double data type)
            printf("value: %f [%s]\n", d, qstoc(QDateTime::fromMSecsSinceEpoch(res["timestamp_ms"].toLongInt()).toString()));
        }
        else // error reading or extracting data from the attribute
            printf("failed: \"%s\"\n", res.toString().c_str());
    }
}
```

## Q.
<a name="state"/>
### Cool. Now, what's the quickest procedure to display a Tango *state* and possibly get the associated color?

## A.

Almost same code as above. Extract color from the "state_color", state string from the "state_string".

```cpp
        success = tw.read_att(td.getDevice(), argv[2], res);  // read attribute, store result in res, return success
        if(success) {
            QuPalette palette;  // default color palette: maps color string to QColor
            QColor color = palette[QString::fromStdString(res["state_color"].toString())];
            Tango::DevState state = static_cast<Tango::DevState>(res["state"].toLongInt());
            qDebug() << __FUNCTION__ << "state color" << color << "state: " << res["value"].toString().c_str()
                     << "as DevState: " << state;
        }
```
## Q.

<a name="cudata_keys"/>
### How did you know that the *res* CuData contained those very keys such as "value", "state_color", "timestamp_ms", and so on.. ?

## A.
There are two ways of knowing what CuData contains. The first is the most universal one, the second is handy when dealing with
Tango specific data.

- simply print the output of CuData::toString() to have a representation of the internal data
- visit the  [CuData bundle description for data exchange with the Tango world](../../cumbia-tango/html/cudata_for_tango.html)
  documentation page.

The example

```cpp
    success = tw.read_att(td.getDevice(), argv[2], res);
    if(success) {
        std::cout << res.toString() << std::endl;
```

executed on TangoTest device/double_scalar attribute, produces an output like this:

|Output |
|-------|
|CuData { ["data_format_str" -> scalar], ["err" -> false], ["mode" -> ], ["msg" -> : Mon Dec 10 15:13:15 2018], ["quality" -> 0], ["quality_color" -> white], ["success_color" -> dark_green], ["timestamp_ms" -> 1544451195642], ["timestamp_us" -> 1544451195.642567], ["value" -> 0.342020], ["w_value" -> 1.000000] } (size: 11 isEmpty: 0)|

from which you can infer that a code like the following can work:

```cpp
    success = tw.read_att(td.getDevice(), argv[2], res);
    bool error = res["err"].toBool();
    if(!error) {
        double read_val = res["value"].toDouble();
        double write_val = res["w_value"].toDouble(); // if Tango attribute is read/write
        std::string message = res["msg"].toString();
    }

```

## Q.

<a name="commands"/>
### I want a quick way to perform a command inout on a device now, thanks.

## A.
Two possible scenarios are possible, dude:

#### 1. You know in advance the output data type and you can provide the input value by code. A full working example follows:

```cpp
#include <cutango-world.h>
#include <tdevice.h>

int main(int argc, char *argv[])
{
    if(argc < 4) {
        printf("usage: %s tango/test/device command input_1\n\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    CuTangoWorld tw;
    CuData res;
    TDevice td(argv[1]);
    if(!td.isValid())
         std::cerr << "failed to connect to device: " << argv[1] << ": " << td.getMessage() << std::endl;
    else {
        res["argins"]  = std::string(argv[3]);                   // input value
        res["in_type"] = Tango::DEV_DOUBLE;      // input data type
        success = tw.cmd_inout(td.getDevice(), argv[2], res);
        std::cout << res["value"].toDouble() << "  [" << res.toString() << "]" << std::endl;
    }
}
```

- Test with: ./doctest test/device/1 DevDouble 10.011

Please note: command and input must be compatible (DEV_DOUBLE has been forced in "in_type" by code)

#### 2. You want to provide a generic input to a command and want it to be converted to the right type at runtime. Look at this:


```cpp

#include <cutango-world.h>
#include <tdevice.h>

int main(int argc, char *argv[])
{
    if(argc < 4) {
        printf("usage: %s tango/test/device  command input_arg1 input_arg2 ... input_argN\n\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    CuTangoWorld tw;
    CuData res;
    TDevice td(argv[1]);
    bool success = td.isValid();
    if(!success)
         std::cerr << "failed to connect to device: " << argv[1] << ": " << td.getMessage() << std::endl;
    else {
        std::vector<std::string> argins;                                // arbitrary number of argins in this example
        for(int i = 3; i < argc; i++)
            argins.push_back(std::string(argv[i]));
        res["argins"] = argins;                                         // store input args in the "argins" key
        success = tw.get_command_info(td.getDevice(), argv[2], res);    // we need this _additional_ call to interpret
        if(success)                                                     // input arguments
            success = tw.cmd_inout(td.getDevice(), argv[2], res);       // get output from command
        std::cout << res.toString() << std::endl;                       // print result
    }
}
```

This example is much more flexible:

- Test with *./doctest test/device/1 DevVarDoubleArray 1  1.2  1.3*

|Output|
|------|
|CuData { ["argins" -> 1,1.2,1.3], ["cmd_name" -> DevVarDoubleArray], ["data_format" -> 1], ["data_format_str" -> vector], ["data_type" -> 13], ["display_level" -> 0], ["err" -> false], ["in_type" -> 13], ["in_type_desc" -> -], ["mode" -> ], ["msg" -> : Mon Dec 10 16:12:43 2018[vector]], ["out_type" -> 13], ["out_type_desc" -> -], ["timestamp_ms" -> 1544454763108], ["timestamp_us" -> 1544454763.108265], ["type" -> property], ["value" -> 1.000000,1.200000,1.300000] } (size: 17 isEmpty: 0)|


- Test with *./doctest test/device/1 DevString "foo bar"*

|Output|
|------|
|CuData { ["argins" -> foo bar], ["cmd_name" -> DevString], ["data_format" -> 0], ["data_format_str" -> scalar], ["data_type" -> 8], ["display_level" -> 0], ["err" -> false], ["in_type" -> 8], ["in_type_desc" -> -], ["mode" -> ], ["msg" -> : Mon Dec 10 16:13:35 2018[scalar]], ["out_type" -> 8], ["out_type_desc" -> -], ["timestamp_ms" -> 1544454815747], ["timestamp_us" -> 1544454815.747049], ["type" -> property], ["value" -> foo bar] } (size: 17 isEmpty: 0)|


## Q.

<a name="cumbiawidget" />
### In QTango there used to be a widget ready to read and display a value. In cumbia there is not. How do I quickly adapt an existing Qt widget?

## A.

#### 1. The longest *reusable* way
Extend the Qt widget apt to display the desired data and implement CuDataListener interface.
See [Quickly add a Qt widget to your cumbia project](../../qumbianewcontrolwizard/html/tutorial_qumbianewcontrolwizard.html) documentation.
Within the *onUpdate(const CuData& data)* method you will receive two kinds of updates:
- if data["type"].toString() == "property" it's a *configuration* update (from the Tango database)
- if data.containsKey("value") you can extract the new value from *data*.


#### 2. The quickest way (what you asked, indeed)

Use a [QuWatcher](../../cumbia-qtcontrols/html/classQuWatcher.html) to monitor a Tango attribute or command, choosing among
the many available signals compatible with the adopted widget.
In this example, we follow the *most generic and flexible* approach, to benefit from all the details available from the update.
Thus, we will use the

*void newData(const CuData &data)* signal from QuWatcher

The QTango implementation adopted a TTextBrowser to read a Tango *DevVarStringArray* and display a dotted list on the text area.
We will employ QTextEdit (object name "textEdit" in *.ui* file) plus QuWatcher to accomplish the same result.

Edit the main widget *cpp* file, add the needed include file and modify the class constructor

```cpp

#include <quwatcher.h>

mywidget::mywidget(CumbiaTango *cut, QWidget *parent) : QWidget(parent)
{
    // ...
    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    // ...

    QuWatcher *quW = new QuWatcher(this, cut, cu_tango_r_fac);
    connect(quW, SIGNAL(newData(const CuData&)), this, SLOT(onNewReport(const CuData&)));
    quW->setSource("$1->GetReport");
}
```

A *Qt SLOT onNewReport()* must be introduced to update the text (heaeder file declaration omitted):

```cpp
void mywidget::onNewReport(const CuData &da)
{
    QString html;
    ui->textEdit->setDisabled(da["err"].toBool());
    ui->textEdit->setToolTip(QString::fromStdString(da["msg"].toString()));
    if(ui->textEdit->isEnabled()) {                                  // read successful
        std::vector<std::string> vs = da["value"].toStringVector();  // DevVarStringArray to vector<string>
        foreach(const std::string &s, vs) {
            html += "<li>" + QString::fromStdString(s) + "</li>\n";  // add list element
        }
        ui->textEdit->setHtml("<ul>" + html + "</ul>");    // wrap <li> into an unordered list (bullet list)
    }
    else {     // read failed
        ui->textEdit->setHtml(ui->textEdit->toolTip());
    }
}

```

## Q.

<a name="configure" />
### I either used QuWatcher or implemented CuDataListener on my custom graphical object. How do I configure it through the Tango database properties (setting maximum and minimum values, display unit and data format)?

## A.
Just look for the CuData with the *type* key set to the *property* string. That's the bundle containing the Tango database
configuration.

```cpp
void MyCustomWidget::onUpdate(const CuData &da)
{
    bool is_config = da["type"].toString() == std::string("property");
    if(is_config) {
        CuVariant m = da["min"], M = da["max"];
        std::string print_format = da["format"].toString();
        double min, max;
        bool ok;
        ok = m.to<double>(min);     // try to convert min
        if(ok)                      // try to convert max if min succeeded
            ok = M.to<double>(max);

        if(ok) {  // very good: Range properly configured in Tango database
            setMinimum(min);  // set minimum value on MyCustomWidget
            setMaximum(max);  // set maximum value on MyCustomWidget
        }

```

## Q.

<a name="tangoprops" />
### How do I get a Tango device property?

## A.

#### 1. The quick way (in current thread)

Build a list of CuData containing the desired property names and types,
as shown in the code below. Then use the CuTangoWorld utility class to
get the properties and finally extract them from the results.

```cpp
CuTangoWorld tw;
CuData res;
std::vector<CuData> in_data;
// 1. build input data with the desired property names
// device property
CuData devpd("device", "test/device/1");
devpd["name"] = "description";
in_data.push_back(devpd);
// attribute property values from the "double_scalar" attribute
CuData apd("device", "test/device/2");
apd["attribute"] = "double_scalar";
apd["name"] = "values";
in_data.push_back(apd);
// class property
CuData cld("class", "TangoTest");
cld["name"] = "ProjectTitle";
in_data.push_back(cld);
// 2. get the properties
tw.get_properties(in_data, res);
// 3. extract  and print results
if(data["err"].toBool())
    printf("error fetching properties: %s\n", data["msg"].toString().c_str());
else {
        printf(PROPERTY|\t\t\t-->|VALUES");
        std::vector<std::string> plist = data["list"].toStringVector();
        for(size_t i = 0; i < plist.size(); i++)
            printf("%s-->%s\n", plist[i].c_str(), data[plist[i]].toString().c_str());
    }
```

#### 2. The *activity* approach (in secondary thread)

- Define a class inheriting from CuDataListener and implement the onUpdate(const CuData&) virtual method
  where results will be delivered. The construction of the input data list is the same as in the example (1).
  The extraction of the results is identical too.

#### The header file
```cpp
class PropertyReader : public CuDataListener
{
     // ...
public:
    // CuDataListener interface
    void onUpdate(const CuData &data);

private:
    CumbiaTango* m_ct;
```

#### The implementation file
We use the CuTDbPropertyReader class to fetch the properties and receive the data when ready.

```cpp
void PropertyReader::get(...) {
    // CuTDbPropertyReader needs a const char* as id as first parameter and a
    // pointer to CumbiaTango as second parameter
    CuTDbPropertyReader *pr = new CuTDbPropertyReader("myPropertyReader", m_ct);
    pr->addListener(this);  // we implement CuDataListener
    pr->get(in_data);       // request properties
}

void PropertyReader::onUpdate(const CuData &data)
{
    // see the point (3) of the *quick way* example above to extract  and print results
}
```

#### Example code
You can find a working command line example under

- *cumbia-libs/cumbia-tango/examples/dbproperties*



## Q.

<a name="except" />
### How to format a message from a Tango *Exception*?

## A.

```cpp
#include<cutango-world.h>
try {
    // ...
}
catch(Tango::DevFailed &e)
{
    CuTangoWorld tw;
    std::string serr = tw.strerror(e);
}

```

## Q.
<a name="multiengine" />
### How to support multiple engines (e.g. Tango and Epics) in the same application?

## A.

The application will make use of the [CumbiaPool](../../cumbia/html/classCumbiaPool.html) class, in combination with CuControlsFactoryPool.
Refer to the [CumbiaPool](../../cumbia/html/classCumbiaPool.html) documentation, that provides an example.
If the *cumbia pool* is configured with appropriate *source patterns*, the application should recognise the engine each source belongs to.
For example, if the *patterns* for a Tango source are *".+/.+"* and *".+->.+"* (as *regular expressions*) and the *patterns* for an
EPICS source include *".+:.+"*, then a source like *sys/tg_test/1/double_scalar* will be interpreted as a Tango source, while
*motor:ai1* will be linked to the EPICS engine. The [CuTangoWorld](../../cumbia-tango/html/classCuTangoWorld.html) and
[CuEpicsWorld](../../cumbia-epics/html/classCuEpicsWorld.html) provide lists of *default source patterns*.
Please note that the "epics" and "tango" strings passed to registerCumbiaImpl, registerImpl and setSrcPatterns must match
for each engine respectively. Those names link together the associated engines.

The *cumbia new project* tool will let you automatically create a skeleton project able to manage multiple engines.

A cumbia application with multiple engine support will generally contain the following initialization code:

```cpp

QumbiaClient::QumbiaClient(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    // ...
{
    // ...

    // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    // m_ctrl_factory_pool  is in this example a private member of type CuControlsFactoryPool
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());   // register EPICS reader implementation
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());   // register EPICS writer implementation

    CuEpicsWorld ew;  // EPICS cumbia helper class
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cumbia_pool->setSrcPatterns("epics", ew.srcPatterns());
#endif

#ifdef QUMBIA_TANGO_CONTROLS
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cumbia_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());  // register Tango writer implementation
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());  // register Tango reader implementation

    CuTangoWorld tw;   // Tango cumbia helper class
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
#endif

    // ...
}
```



