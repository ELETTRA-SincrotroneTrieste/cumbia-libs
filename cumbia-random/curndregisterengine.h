#ifndef CURNDREGISTERENGINE_H
#define CURNDREGISTERENGINE_H

#include <cumbiarandom.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>

class CuRndRegisterEnginePrivate;

class CuRndRegisterEngine
{
public:
    CuRndRegisterEngine();

    virtual ~CuRndRegisterEngine();

    CumbiaRandom *registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo);
private:
    CuRndRegisterEnginePrivate *d;
};

#endif // CURNDREGISTERENGINE_H
