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
