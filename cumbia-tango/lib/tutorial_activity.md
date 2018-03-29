# Writing an activity to read Tango device, attribute and class properties    {#tutorial_activity}

In this tutorial, we will learn how to write an activity to read Tango
attribute, device and class properties from the Tango database.
The example discussed here makes already part of the library, but it can be used 
to create custom activities to automate operations or to simply understand how to write
*activities*.

We need to recall cumbia CuActivity concepts and will be inspired by CuTReader
implementation.

CuTReader implements the CuTangoActionI interface, which in turn derives from CuThreadListener
so as to force subclasses to provide the

\li CuThreadListener::onProgress
\li CuThreadListener::onResult
\li CuThreadListener::getToken

methods.
CuTReader inherits from the CuTangoActionI interface in order to integrate with CumbiaTango and the
*action factory* service, that allows to provide a *per device* thread organisation. Since in our case
we deal with database properties, we choose a different approach, since not every action has to do with
a device (for example, a class property is not related to a device).

With <em>qtcreator</em>, we add a new C++ class, name it CuTDbPropertyReader and let it inherit
from the CuThreadListener interface.
Once the file is created and opened in the editor, right click on the CuTDbPropertyReader class
definition, choose <em>refactor</em> and then <em>Insert virtual functions of base classes</em>.


The cutdbpropertyreader.h file will look like this:

\code
#ifndef CUTDBPROPERTYREADER_H
#define CUTDBPROPERTYREADER_H

#include <cuthreadlistener.h>.h>

class CuTDbPropertyReader : public CuThreadListener
{
public:
    CuTDbPropertyReader();
    virtual ~CuTDbPropertyReader();

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    CuData getToken() const;
};

#endif // CUTDBPROPERTYREADER_H

\endcode

Right clicking on every method definition, let qtcreator create empty bodies for each function.

### Note
CuTDbPropertyReader will be a proxy to the *activity* fetching the data from the database, so that clients
do not have to directly deal with the activity.

CuTangoActionI *Type* enum defines a few fundamental types of actions that can be implemented. We will use the *DbReadProp*.

\code 
CuTangoActionI::Type CuTDbPropertyReader::getType() const
{
    return CuTangoActionI::DbReadProp;
}
\endcode

### Bridge pattern
Let's adopt the *bridge* pattern, also known as *d* private pointer by the Qt developers, so that CuTDbPropertyReader can be extended
in the future without causing troubles to binary compatibility. To accomplish this, add the line

\code
class CuTDbPropertyReaderPrivate;
\endcode

at the top of the header file and the d private class attribute:

\code
private:
    CuTDbPropertyReaderPrivate *d;
\endcode

Let's move to the CPP file, define the private class and don't forget to allocate it in the constructor.
We need to include some header files to provide needed definitions.
*cumbia* library provides an object that bundles keys to values: CuData. We can use it to provide a list of 
database device, attribute or class properties that we want to fetch, *in_props*.
A set of CuDataListener objects is stored within the *listeners* variable.

\code
#include "cutdbpropertyreader.h"
#include <set>
#include <list>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <culog.h>
#include <cuactivity.h>
#include <cumbiatango.h>

class CuTDbPropertyReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    std::list<CuData> in_props;
    std::string id;
    CumbiaTango *cumbia_t;
    CuGetTDbPropActivity *activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;
};
\endcode

*cumbia* is a reference to the CumbiaTango engine.
Cumbia base would be enough for this simple case, where there's no thread sharing across different instances of CuTDbPropertyReader.
Nonetheless, since every cumbia application connecting to Tango instantiates CumbiaTango, we use it because methods to provide the
thread factory and the event bridge in use (see Cumbia::registerActivity comments below).

### The class constructor.

CuTDbPropertyReader must grab a reference to Cumbia and a list of desired database properties must be provided at some time.
We decide to pass the Cumbia reference in the constructor, together with an *id* that will identify every CuTDbPropertyReader
used in the application (see "Registering *CuGetTDbPropActivity* with Cumbia" paragraph).  We provide the list of desired properties in the *get* method instead.

An additional method called addListener is added in order to let objects implementing CuDataListener 
receive new data from the worker thread.

\code
#include <list>

class CuTDbPropertyReaderPrivate;
class Cumbia;

class CuTDbPropertyReader : public CuThreadListener
{
public:
    CuTDbPropertyReader(const std::string& id, Cumbia *cumbia);

    void get(const std::list<CuData>& in_data);
    
    void addListener(CuDataListener *l);
    
    //...

\endcode

In the .cpp:

\code
CuTDbPropertyReader::CuTDbPropertyReader(const std::string &id, CumbiaTango *cumbia)
{
    d = new CuTDbPropertyReaderPrivate;
    d->cumbia_t = cumbia;
    d->id = id;
}

void CuTDbPropertyReader::get(const std::list<CuData> &in_data)
{

}


void CuTDbPropertyReader::addListener(CuDataListener *l)
{
    d->listeners.insert(l);
}

\endcode

### The class destructor
Don't forget to delete the d pointer in the destructor:
\code
CuTDbPropertyReader::~CuTDbPropertyReader()
{
    delete d;
}
\endcode


The *get* method accept a list of desired properties and fetches them from the Tango database.
We need a *cumbia activity*, an implementation of *CuActivity*, to carry out the task in a separate thread
and post the result through the *onResult* method of CuTDbPropertyReader.
Let's call it *CuGetTDbPropActivity*, that stands for *(cumbia) get Tango database property activity. It implements CuActivity.
With the help of *qtcreator*, add a new class with that name. Then, as we did before, add necessary includes and 
virtual method definitions (.h) and implementation (.cpp).
We like to have a D pointer store the private fields of the class, as we did before.

#### getdbpropactivity.h

\code
#ifndef GETTDBPROPACTIVITY_H
#define GETTDBPROPACTIVITY_H

#include <cuactivity.h>
#include <list>

class CuGetTDbPropActivityPrivate;

class CuGetTDbPropActivity : public CuActivity
{
public:
    CuGetTDbPropActivity(const std::list<CuData> &in_data);
    virtual ~CuGetTDbPropActivity(); // class destructor: delete the D pointer!
    
    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;
protected:
    void init();
    void execute();
    void onExit();
private:
    CuGetTDbPropActivity *d;
};

#endif // GETTDBPROPACTIVITY_H
\endcode

Right clicking on every method definition in the .h file, you can add an empty implementation in the corresponding cpp.

At this point, we have the structure of an object that clients will use to fetch Tango database properties (proxy) and the
activity that will carry out the actual task in a separate thread.
Let the CuGetTDbPropActivity constructor accept the list of properties to fetch.

\code
#include "cugettdbpropactivity.h"

class CuGetTDbPropActivityPrivate
{
public:
    std::list<CuData> in_data;
};

CuGetTDbPropActivity::CuGetTDbPropActivity(const std::list<CuData> &in_data)
{
    d = new CuGetTDbPropActivityPrivate;
    d->in_data = in_data;
}

CuGetTDbPropActivity::~CuGetTDbPropActivity()
{
    delete d;
}
\endcode


*cumbia-tango* library keeps the *Tango* aware code in a header/cpp couple of files named *tango-world.h/.cpp*. There goes the 
code accessing Tango functions, in order to keep them off other places within *cumbia* code. In your case, you can integrate 
Tango calls in your *activity* implementation.

### Defining the desired property list.
A list of *CuData* will contains the names and the types of properties we want to fetch.

*Every CuData must contain*:
\li the key "*type*" with one of the values *device*, *device_attribute* [=*attribute*], *class_attribute*, *class*;
\li the key "*name*" with the name of the property.

The list of *CuData* will be passed to the CuTDbPropertyReader::get method.

### Registering *CuGetTDbPropActivity* with Cumbia
Every activity must be registered with cumbia, so that it can be searched by token, paused and resumed (if it's a *continuous* activity), and finally 
unregistered. Cumbia searches amongst all the registered threads to see if one with the same *token* is already there. If not, a new one is created 
and started. For example, in CuTReader a thread token is created with the "device" key associated to the Tango device name. In this way, Cumbia reuses 
threads working on the same device.
To do so, a Cumbia method is available: 

\code
void registerActivity(CuActivity *activity, 
   CuThreadListener *dataListener, 
   const CuData& thread_token, 
   const CuThreadFactoryImplI& thread_factory_impl, 
   const CuThreadsEventBridgeFactory_I& eventsBridgeFactoryImpl);
\endcode

\li activity is the CuActivity implementation (CuGetTDbPropActivity);
\li dataListener is the *activity data listener*;
\li thread_factory_impl is a factory that provides a thread implementation. We will use CuThread, cumbia C++ 11 thread.
\li eventsBridgeFactoryImpl a bridge that is used to post events from the *activity thread* to the *main thread*. Within a Qt application,
    Qt event loop can be used, in conjunction with QApplication::postEvent.
    
CumbiaTango provides methods to get the thread factory and the event bridge in use.

Let's then  mix these ingredients to register our activity in the CuTDbPropertyReader::get method:

\li two more include files must be added:
\code
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>

#include "gettdbpropactivity.h" // our activity
\endcode

\li Activity is instantiated and registered. The registerActivity method requires a *thread token*. The thread token makes a thread shareable 
between activities. There is no particular reason to share a thread across multiple CuGetTDbPropActivity instances. So we ask clients to provide
a unique *id* in the CuTDbPropertyReader constructor, as aforementioned. It was stored in the id string field of CuTDbPropertyReaderPrivate.

\code
void CuTDbPropertyReader::get(const std::list<CuData> &in_data)
{
    CuData thread_tok("id", d->id);  // build the thread token using the id
    d->activity = new CuGetTDbPropActivity();  // instantiate activity
    // get application wide thread events bridge factory (typically, Qt event loop/postEvent in Qt QApplication)
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    // get application wide thread factory implementation (typically, CuThread's)
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, thread_tok, fi, bf);
}
\endcode

Cumbia *registerActivity* creates a new thread and starts it. At this point, we must write the code to fetch the properties from the database inside the activity
three entry points: init, execute and onExit.


### Doing the work inside the activity
Database properties are fetched in a separate thread. Let's have a look at the CuGetTDbPropActivity three *worker* methods.
The list of properties to get is stored in the in_data field of the CuGetTDbPropActivityPrivate object.
We make use of the method CuTangoWorld::get_properties, written in the library file cutango-world.cpp. It accepts as input a list of CuData and a reference
to a CuData where results are placed. A tango database host can be optionally specified as last parameter.

get_properties method definition, from cutango-world.cpp

\code
bool CuTangoWorld::get_properties(const std::list<CuData> &in_list, CuData &res, const std::string& dbhost)
\endcode

All the work is carried out by this method, so let's call it from within the *execute* method of CuGetTDbPropActivity, in the cugettdbpropactivity.cpp file:

\code
#include <cutango-world.h>
#include <cumacros.h> // printf, perr...
\endcode

Using the CuTangoWorld class helper we fetch the properties, that are saved within res, of type CuData.
Finally, to make the result available on the main thread, we must remember to call publishResult.

\code
void CuGetTDbPropActivity::execute()
{
    CuTangoWorld tw;
    CuData res;
    tw.get_properties(d->in_data, res);
    publishResult(res);
}
\endcode

### Testing the code
We have now to test the written code. To accomplish this, with qt creator we create a plain C++ application. The project name can be "dbproperties". We will write a command line utility to
print the device/class/attribute properties on the command line.
Open the dbproperties.pro file: we need to add the include path and libraries for cumbia and cumbia tango

\code
#
# dbproperties.pro
INCLUDEPATH += /usr/local/include/cumbia /usr/local/include/cumbia-tango
LIBS += -L/usr/local/lib -lcumbia-tango
\endcode

Create the project and then add a new class named "PropertyReader".

Let PropertyReader inherit from CuDataListener and add the pure virtual method onUpdate implementation with qt creator.
Its constructor can have empty arguments, while a *get* method will accept an *id* as string and the list of required 
properties and use a CuTDbPropertyReader to fetch them. *onUpdate* will receive the results.

Let's start having a look at the skeleton of the PropertyReader class created with qt creator.

#### File propertyreader.h

\code
#include <cudatalistener.h>
#include <vector>
#include <string>

class PropertyReader : public CuDataListener
{
public:
    PropertyReader(const std::vector<std::string> props);

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
};

\endcode


#### File propertyreader.cpp

Add the necessary includes to the .cpp file

\code
#include "propertyreader.h"
#include <cutdbpropertyreader.h>
#include <cumacros.h>
#include <cudata.h>
#include <algorithm> // for find in vector
#include <list>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridge.h>
#include <cumbiatango.h>
\endcode

As required by cumbia applications, a CumbiaTango object must be instantiated for Tango clients. Since cumbia is 
*multi threaded*, we must pass a C++ thread implementation to the CumbiaTango constructor, as well as a so called
*bridge* that is used to forward events from the secondary thread (*worker*) to the main one. Cumbia offers base
implementations of both objects. For Qt applications, we would use *QThreadsEventBridge*, that employs Qt QApplication
and QApplication's *postEvent* as a means to deliver events between separate threads. For non Qt applications, we will
use base event loop and event bridge provided by cumbia, namely CuThreadsEventBridge. For both Qt and non Qt applications,
CuThread (cumbia C++11 thread implementation) are normally used. These two objects that parametrize CumbiaTango are not 
directly instantiated by the client, but rather through a couple of factories. In the Qt/QApplication scenario, QApplication
transparently runs an event loop. In our simple console application, we must start the cumbia base event loop.
This is accomplished in the PropertyReader constructor as follows:

\code
PropertyReader::PropertyReader()
{
    m_ct = new CumbiaTango(new CuThreadFactoryImpl(), new CuThreadsEventBridgeFactory());
    m_ct->getServiceProvider()->registerService(CuServices::EventLoop, new CuEventLoopService());
    /* start the event loop in a separate thread (true param), where data from activities will be posted */
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->exec(true);
}
\endcode

The event loop is a cumbia *service*. Cumbia avoids *singleton* patterns and it rather uses the service provider pattern
to supply application wide access to "services" from anywhere. CuServiceProvider is where services are registered and
then fetched (see the *registerService* and *get* methods in the snipped above.
The event loop is <em>exec</em>uted at last.

*m_ct* is a CumbiaTango class attribute, that is declared within PropertyReader class definition.

#### File main.cpp  
Let's now examine the main.cpp of this application before going back to the propertyreader.cpp.
Properties are saved into a vector of strings by parsing the command line arguments. Then a PropertyReader is asked to fetch
them from the Tango database.

\code

#include "propertyreader.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> props;
    for(int i = 1; i < argc; i++)
        props.push_back( std::string(argv[i]));
    PropertyReader pr;
    pr.get("pr1", props);
}


\endcode

#### File propertyreader.cpp (2)

The *get* method of the PropertyReader class will be now discussed. The first argument is an *id* that helps cumbia 
execute several property readers in different threads. The second is a list of strings with the desired properties.
Basically, in the *for* loop, the program tries to distinguish between device, attribute and class property according to
the expression provided in the command line. The application recognizes these three forms:

\li test/device/1:description to fetch the device property *description* from the device test/device/1 
\li test/device/1/double_scalar:values to fetch the *values* attribute property from device test/device/1 and attribute double_scalar
\li TangoTest:Description to fetch the *Description* property from the TangoTest class.

*get* parses the queries and adds a descriptive CuData to a list called *in_list*.

When the list is ready, a CuTDbPropertyReader is created, addListener is called to get updates and finally CuTDbPropertyReader::get is 
called with the string *id* and the list as parameters.

Since CuTDbPropertyReader works in a separate thread, we must keep the current one waiting for the results. For this, right after the
CuTDbPropertyReader::get call, we ask the event loop to wait for the *exit signal*. The *exit signal* is invoked by the 
PropertyReader::onUpdate method, once data has been received and displayed.

\code

// test/device/1:description device property: two '/' and ':'
// test/device/1/double_scalar:values attribute properties: three '/' and one ':'
// TangoTest:Description class property: one '/'
void PropertyReader::get(const char *id, const std::vector<std::string> &props)
{
    /* start the event loop in a separate thread, where data from activities will be posted */
    std::list<CuData> in_data;
    for(size_t i = 0; i < props.size(); i++) {
        size_t cnt = count(props[i].begin(), props[i].end(), '/');
        size_t cpos = props[i].find(':');
        if(cnt == 2 && cpos < std::string::npos)
        {
            CuData devpd("device", props[i].substr(0, cpos));
            devpd["name"] = props[i].substr(cpos + 1, std::string::npos);
            in_data.push_back(devpd);
        }
        else if(cnt == 3) {
            CuData devpd("device", props[i].substr(0, props[i].rfind('/')));
            if(cpos < std::string::npos) {
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos - props[i].rfind('/') -1);
                devpd["name"] = props[i].substr(cpos + 1, std::string::npos);
            }
            else
                devpd["attribute"] = props[i].substr(props[i].rfind('/') + 1, cpos); // cpos == npos

            in_data.push_back(devpd);
        }
        else if(cnt == 0 && cpos < std::string::npos) { // class
            CuData cld("class", props[i].substr(0, cpos));
            cld["name"] = props[i].substr(cpos + 1);
            in_data.push_back(cld);
        }
    }
    CuTDbPropertyReader *pr = new CuTDbPropertyReader(id, m_ct);
    pr->addListener(this);
    pr->get(in_data);
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->wait();
    delete m_ct;
    delete pr;
}

\endcode

As you can see, after *wait*ing, we proceed to delete the CumbiaTango and the CuTDbPropertyReader.

The *onUpdate* method below simply extracts data from the CuData received and prints it nicely on the console.
Finally, *exit* is called on the event loop.

\code
void PropertyReader::onUpdate(const CuData &data)
{
    if(data["err"].toBool())
        printf("\n\e[1;31m** \e[0m error fetching properties: \e[1;31m%s\e[0m\n", data["msg"].toString().c_str());
    else
        printf("\n\e[1;32m** %45s     VALUES\e[0m\n", "PROPERTIES");
    std::vector<std::string> plist = data["list"].toStringVector();
    for(size_t i = 0; i < plist.size(); i++)
        printf("\e[1;32m--\e[0m %55s \e[1;33m--> \e[0m%s\n", plist[i].c_str(), data[plist[i]].toString().c_str());
    static_cast<CuEventLoopService*>(m_ct->getServiceProvider()->get(CuServices::EventLoop))->exit();
}
\endcode

The data handed to the *onUpdate* method contains a "list" key which value is a list of strings containing all the names of the properties
stored in the CuData. Iterating on such list we are able to print the value of each property as a string.


### Application output example

When the *dbproperties* application is launched with this command:

\code

./dbproperties test/device/1:description test/device/1/double_scalar:unit test/device/1/double_scalar:max_alarm TangoTest:Csoglia TangoTest:doc_url test/device/1:helperApplication test/device/1:values test/device/2/double_scalar:values
\endcode

the output is like this:

\code

**                                    PROPERTIES     VALUES
--                        test/device/1/double_scalar:unit --> Pippeross
--                               test/device/1:description --> This is a Tango Test device with ,many interesing attributes and commands
--                         test/device/1:helperApplication --> atkpanel
--                                    test/device/1:values --> 10,20,30,40,50
--                                       TangoTest:Csoglia --> 5
--                                       TangoTest:doc_url --> http://www.esrf.eu/computing/cs/tango/tango_doc/ds_doc/

\endcode

As you can see, there is no *test/device/2/double_scalar:values* property in the tango database at the moment.

If you want to know all the properties of an attribute *and* the *tango class* "Description", you will execute:

\code
./dbproperties test/device/1/long_scalar TangoTest:Description
\endcode

and get this result:

\code

**                                    PROPERTIES     VALUES
--                    test/device/1/long_scalar:abs_change --> 4
--            test/device/1/long_scalar:archive_abs_change --> 1
--                test/device/1/long_scalar:archive_period --> 2000
--            test/device/1/long_scalar:archive_rel_change --> 1
--                   test/device/1/long_scalar:displayMask --> 0,3,5,1
--                  test/device/1/long_scalar:display_unit --> tapiocas
--                  test/device/1/long_scalar:falseStrings --> f0,f3,f5,f1
--                     test/device/1/long_scalar:max_alarm --> 100
--                     test/device/1/long_scalar:max_value --> 280
--                   test/device/1/long_scalar:max_warning --> 100
--                     test/device/1/long_scalar:min_alarm --> 1
--                     test/device/1/long_scalar:min_value --> 0
--                   test/device/1/long_scalar:min_warning --> 1
--                    test/device/1/long_scalar:numColumns --> 4
--                       test/device/1/long_scalar:numRows --> 1
--                    test/device/1/long_scalar:rel_change --> 3
--                 test/device/1/long_scalar:standard_unit --> tapiocasStdUnit
--                   test/device/1/long_scalar:trueStrings --> v0,v3,v5,v1
--                          test/device/1/long_scalar:unit --> tapiocasUnit
--                        test/device/1/long_scalar:values --> Pippo,Pluto,Minnie,Briscola,Re,Torre,Asso,Gemello,Ariete,Toro,Ponyo
--                                   TangoTest:Description --> A device to test generic clients. It offers a "echo" like command for,each TANGO data type (i.e. each command returns an exact copy of <argin>).

\endcode


### Final observations
In this tutorial we've learnt how to deal with writing a simple cumbia *activity*.
The client of the activity, the *property reader* example, is a little bit complicated by the need of dealing with 
activities that work in a separate thread. For this, we had to use cumbia *event loop* implementation, calling *execute*, 
*wait* and *exit* at the right moment. Generally, activities are thought to be used within a multi threaded environment
where a event loop is given *for free*, as it is in Qt's *QApplication*. 
In that case, the programmer's work is much easier.






