#include "cutangoregisterengine.h"
#include "cutcontrolswriter.h"
#include <cutthread.h>
#include <qthreadseventbridge.h>
#include <cutcontrolsreader.h>
#include <cutango-world.h>
#include <qthreadseventbridgefactory.h>

class CuTangoRegisterEnginePrivate {
  public:
};

CuTangoRegisterEngine::CuTangoRegisterEngine() {
    d = new CuTangoRegisterEnginePrivate;
}

CuTangoRegisterEngine::~CuTangoRegisterEngine() {
    delete d;
}

CumbiaTango *CuTangoRegisterEngine::registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo) {
    CumbiaTango *cuta = new CumbiaTango(new CuTThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    fpoo.registerImpl("tango", CuTReaderFactory());
    fpoo.registerImpl("tango", CuTWriterFactory());
    CuTangoWorld tw;
    fpoo.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    return cuta;
}
