#ifndef CUEPICSACTIONI_H
#define CUEPICSACTIONI_H

#include <epsource.h>
#include <cuthreadlistener.h>

class CuDataListener;
class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CumbiaEpics;
class CuData;

/*! \brief an interface for an EPICS *action*, as a reader (implemented) or a writer (not yet
 *         implemented)
 *
 * A CuEpicsActionI describes what readers or writers usually do. They do *actions*, and they
 * must adhere to this interface that requires to
 *
 * \li add or remove data listeners, that are updated by CuThreadListener::onProgress and
 *     CuThreadListener::onResult
 * \li declare the type of action (Reader, Writer, ... - see the Type enum)
 * \li provide a start and a stop method where activities are instantiated and registered with
 *     Cumbia::registerActivity and finally unregistered with Cumbia::unregisterActivity
 * \li provide an exiting method
 *
 * \par Examples
 * CuMonitor
 */
class CuEpicsActionI :  public CuThreadListener
{
public:

    virtual ~CuEpicsActionI() {}

    enum Type { Reader = 0, Writer, MultiReader, MultiWriter, PropConfig };

    virtual EpSource getSource() const  = 0;

    virtual Type getType() const = 0;

    virtual void addDataListener(CuDataListener *l) = 0;

    virtual void removeDataListener(CuDataListener *l) = 0;

    virtual size_t dataListenersCount() = 0;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void sendData(const CuData& data) = 0;

    virtual void getData(CuData& d_inout) const = 0;

    virtual bool exiting() const = 0;
};

#endif // CUEPICSACTIONI_H
