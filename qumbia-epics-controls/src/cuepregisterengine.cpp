#include "cuepcontrolsreader.h"
#include "cuepcontrolswriter.h"
#include "cuepregisterengine.h"
#include "cuepics-world.h"

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

class CuEpRegisterEnginePrivate {
public:
};

CuEpRegisterEngine::CuEpRegisterEngine()
{
    d = new CuEpRegisterEnginePrivate;
}

CuEpRegisterEngine::~CuEpRegisterEngine() {
    delete d;
}

CumbiaEpics *CuEpRegisterEngine::registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo)
{
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    fpoo.registerImpl("epics", CuEpReaderFactory());
    fpoo.registerImpl("epics", CuEpWriterFactory());
    CuEpicsWorld ew;
    fpoo.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
    return cuep;
}
