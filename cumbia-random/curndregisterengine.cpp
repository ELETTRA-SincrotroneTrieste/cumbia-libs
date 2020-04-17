#include "cumbiarndworld.h"
#include "curndreader.h"
#include "curndregisterengine.h"

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

class CuRndRegisterEnginePrivate {
public:
};

CuRndRegisterEngine::CuRndRegisterEngine() {
    d = new CuRndRegisterEnginePrivate;
}

CuRndRegisterEngine::~CuRndRegisterEngine() {
    delete d;
}

CumbiaRandom *CuRndRegisterEngine::registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo) {
    CumbiaRandom *cura = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaRNDWorld rndw;
    cu_pool->registerCumbiaImpl("random", cura);
    fpoo.registerImpl("random", CuRNDReaderFactory());
    fpoo.setSrcPatterns("random", rndw.srcPatterns());
    cu_pool->setSrcPatterns("random", rndw.srcPatterns());
    return cura;
}
