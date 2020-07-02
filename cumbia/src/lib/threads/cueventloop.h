#ifndef CUEVENTLOOP_H
#define CUEVENTLOOP_H

#include <stdlib.h>
#include <cuservicei.h>

class CuData;
class CuEventI;
class CuEventLoopPrivate;
class CuELoopEventPrivate;
class CuEventI;

/*! \brief listens for the events from the CuEventLoopService
 *
 * When an event occurs, CuEventLoopService invokes the CuEventLoopListener::onEvent
 * callback
 *
 */
class CuEventLoopListener
{
public:

    virtual ~CuEventLoopListener() {}

    /*! \brief callback invoked when an event is delivered from the
     *         event loop service
     *
     * @param e the event sent by CuEventLoopService::postEvent
     *
     * \par Implementation
     * CuThreadsEventBridge implements CuEventLoopListener
     *
     * @see CuEventLoopService::setCuEventLoopListener
     */
    virtual void onEvent(CuEventI *e) = 0;
};

/*! \brief service that implements a simple event loop for cumbia
 *         applications.
 *
 * CuEventLoopService is a cumbia *service* (CuServiceI) that offers
 * a simple *event loop* for an application.
 * Please note that <strong>if you are developing a Qt application</strong>
 * you should employ Qt's QApplication event loop (read CuThreadsEventBridge
 * and Cumbia::registerActivity documentation).
 *
 * See also the CuServiceProvider documentation.
 */
class CuEventLoopService : public CuServiceI
{
public:

public:
    CuEventLoopService(CuEventLoopListener *l = nullptr);

    virtual ~CuEventLoopService();

    void exec(bool threaded = false);

    void postEvent(CuEventLoopListener *l, CuEventI* e);

    void addCuEventLoopListener(CuEventLoopListener *l);

    void removeCuEventLoopListener(CuEventLoopListener *l);

    void exit();

    void wait();

protected:
    virtual void run();

private:
    CuEventLoopPrivate *d;

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;
};

#endif // CUEVENTLOOP_H
