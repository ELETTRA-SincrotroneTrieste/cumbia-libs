#ifndef CUEPREGISTERENGINE_H
#define CUEPREGISTERENGINE_H

#include <cumbiaepics.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>

class CuEpRegisterEnginePrivate;

class CuEpRegisterEngine
{
public:
    CuEpRegisterEngine();
    virtual ~CuEpRegisterEngine();
    CumbiaEpics *registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo);

private:
    CuEpRegisterEnginePrivate *d;
};

#endif // CUEPREGISTERENGINE_H
