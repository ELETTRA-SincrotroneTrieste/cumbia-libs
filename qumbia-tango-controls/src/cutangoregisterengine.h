#ifndef CUTANGOREGISTERENGINE_H
#define CUTANGOREGISTERENGINE_H

#include <cumbiatango.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>

class CuTangoRegisterEnginePrivate;

class CuTangoRegisterEngine
{
public:
    CuTangoRegisterEngine();

    virtual ~CuTangoRegisterEngine();

    CumbiaTango *registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo);
private:
    CuTangoRegisterEnginePrivate *d;
};

#endif // CUTANGOREGISTERENGINE_H
