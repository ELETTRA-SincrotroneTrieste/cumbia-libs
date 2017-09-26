#ifndef CUEPICSACTIONI_H
#define CUEPICSACTIONI_H

#include <epsource.h>
#include <cuthreadlistener.h>

class CuDataListener;
class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CumbiaEpics;
class CuData;

class CuEpicsActionI :  public CuThreadListener
{
public:

    virtual ~CuEpicsActionI() {}

    enum Type { Reader = 0, Writer, MultiReader, MultiWriter, AttConfig };

    virtual EpSource getSource() const  = 0;

    virtual Type getType() const = 0;

    virtual void addDataListener(CuDataListener *l) = 0;

    virtual void removeDataListener(CuDataListener *l) = 0;

    virtual size_t dataListenersCount() = 0;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void sendData(const CuData& data) = 0;

    virtual void getData(CuData& d_inout) const = 0;

};

#endif // CUEPICSACTIONI_H
