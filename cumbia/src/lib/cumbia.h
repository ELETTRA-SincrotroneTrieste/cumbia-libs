#ifndef CUMBIA_H
#define CUMBIA_H

class CuServiceProvider;
class CumbiaPrivate;
class CuThreadListener;
class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuData;
class CuActivity;
class CuContinuousActivity;
class CuActivityEvent;

/** \mainpage
 *
 * Cumbia is a library that offers a carefree approach to multi thread application design and implementation.
 * The user writes \b Activities and decides when their instances are started and to which thread they belong.
 * A \b token is used to register an \b Activity and activities registered with the same token are run in
 * the same thread. Work is done inside the \a init, \a execute and \a exit methods of an \a Activity.
 * The library guarantees that the aforementioned methods are always called in the activity thread.
 * From within \a init, \a execute and \a exit, computed results can be forwarded to the main execution thread,
 * where they can be used to update a graphical interface.
 *
 * In asynchronous environments, threads have always posed some kind of challenge for the programmer. Shared data,
 * message exchange, proper termination are some aspects that cannot be overlooked. The \a Android \a AsyncTask offers a
 * simple approach to writing code that is executed in a separate thread.
 * The API provides a method that is called in the secondary thread context and functions to post results on the main one.
 *
 * \par Acvivities
 * Cumbia \a CuActivity’s purpose is to replicate the carefree approach supplied by the \a AsyncTask.
 * In this respect, a \a CuActivity is an interface to allow subclasses do work within three specific
 * methods: init, execute and onExit. Therein, the code is run in a separate thread. The \a publishProgress
 * and \a publishResult methods hand data to the main thread. To accomplish all this, an event loop must be running.
 * By an initial parametrization, either a custom one (such as Qt’s, used in qumbia-qtcontrols) or the builtin
 * cumbia \a CuEventLoop can be installed.
 * New activities must be registered in the \a CuActivityManager service, and unregistered when they are no
 * longer needed. In this way, a \a token can be used to group several activities by a smaller number of threads.
 * In other words, <em>activities with the same token run in the same thread</em>.
 *
 * \par Services
 * By means of the reference to the \a Cumbia instance, that must be maintained throughout the entire life of an application,
 * you can access services. They are registered in the \a CuServiceProvider and accessed by name. The activity manager,
 * the thread and the log services are some examples, but others can be written and installed, as long as they adhere
 * to the \a CuServiceI interface (e.g \a cumbia-tango’s \a CuActionFactoryService and \a CuDeviceFactoryService).
 * \a Cumbia can be subclassed in order to provide additional features specific to the engine employed.
 * \a CumbiaPool allows to register and use multiple engines in the same application.
 *
 * \par Data interchange
 * Data transfer is realised with the aid of the \a CuData and \a CuVariant classes. The former is a bundle pairing keys to values.
 * The latter memorises data and implements several methods to store, extract and convert it to different types and formats.
 * The \a cumbia-qtcontrols module handles these structures to provide a primary data display facility, unaware of the specific
 * engine underneath (\a Tango, \a Epics, ...)
 *
 */
class Cumbia
{

public:
    enum Type { CumbiaBaseType = 0, CumbiaUserType = 10 };

    Cumbia();

    CuServiceProvider *getServiceProvider() const;

    virtual ~Cumbia();

    void registerActivity(CuActivity *activity,
                          CuThreadListener *dataListener,
                          const CuData& thread_token,
                          const CuThreadFactoryImplI& thread_factory_impl,
                          const CuThreadsEventBridgeFactory_I& eventsBridgeFactoryImpl);

    void unregisterActivity(CuActivity *activity);

    CuActivity *findActivity(const CuData& token) const;

    bool threadRelease(CuThreadListener *l);

    void setActivityPeriod(CuActivity *a, int timeout);

    unsigned long getActivityPeriod(CuActivity *a) const;

    void pauseActivity(CuActivity *a);

    void resumeActivity(CuActivity *a);

    void postEvent(CuActivity *a, CuActivityEvent *e);

    void finish();

    virtual int getType() const;

private:
    CumbiaPrivate *d;

};

#endif // CUMBIA_H
