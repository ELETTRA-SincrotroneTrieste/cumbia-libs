#include "cumbiasupervisor.h"
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>

#ifdef QUMBIA_EPICS_CONTROLS
#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#include <cuepreadoptions.h>
#endif

CumbiaSupervisor::CumbiaSupervisor()
{
    cu_pool = nullptr;
}

void CumbiaSupervisor::setup()
{
    if(cu_pool) {
        perr("CumbiaSupervisor.setup: cumbia already setup");
    }
    else {
        cu_pool = new CumbiaPool();
        // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS
        CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("epics", cuep);
        // m_ctrl_factory_pool  is in this example a private member of type CuControlsFactoryPool
        ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());   // register EPICS reader implementation
        ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());   // register EPICS writer implementation
        CuEpicsWorld ew;  // EPICS cumbia helper class
        ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
        cu_pool->setSrcPatterns("epics", ew.srcPatterns());
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("tango", cuta);
        ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());  // register Tango writer implementation
        ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());  // register Tango reader implementation
        CuTangoWorld tw;   // Tango cumbia helper class
        ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
        cu_pool->setSrcPatterns("tango", tw.srcPatterns());
#endif
    }
}

void CumbiaSupervisor::dispose()
{
    if(cu_pool) {
        Cumbia *cumb =  cu_pool->get("tango");
        printf("deleting cumbia tango %p in dispose()\n", cumb);
        if(cumb) delete cumb;
        cumb =  cu_pool->get("epics");
        if(cumb) delete cumb;
    }
    delete cu_pool;
    cu_pool = nullptr;
}

