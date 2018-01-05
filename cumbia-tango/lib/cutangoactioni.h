#ifndef CUTANGOACTIONI_H
#define CUTANGOACTIONI_H

#include <tsource.h>
#include <cuthreadlistener.h>

class CuDataListener;
class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CumbiaTango;
class CuData;

class CuTangoActionI :  public CuThreadListener
{
public:

    virtual ~CuTangoActionI() {}

    enum Type { Reader = 0, Writer, AttConfig, DbReadProp, DbWriteProp };

    virtual TSource getSource() const  = 0;

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

#endif // CUTANGOACTIONI_H
