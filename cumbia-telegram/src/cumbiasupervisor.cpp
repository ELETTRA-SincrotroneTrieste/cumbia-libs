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

#include <QStringList>

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

        // Tango attribute patterns
        // (?:[A-Za-z0-9]+\:\d+)/[A-Za-z0-9_\.]+/[A-Za-z0-9_\.]+/[A-Za-z0-9_\.]+/[A-Za-z0-9_\.]+
        const char *h_p = "(?:[A-Za-z0-9]+\\:\\d+)"; // host pattern e.g. hokuto:20000
        const char *t_p = "[A-Za-z0-9_\\.]+"; // t_p tango pattern
        QString a_p = QString("%1/%1/%1/%1").arg(t_p); // a_p  attribute pattern
        QString h_a_p = QString("%1/%2/%2/%2/%2").arg(h_p).arg(t_p);
        std::vector<std::string> tg_patterns;
        tg_patterns.push_back(h_a_p.toStdString());
        tg_patterns.push_back(a_p.toStdString());

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("tango", cuta);
        ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());  // register Tango writer implementation
        ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());  // register Tango reader implementation
        ctrl_factory_pool.setSrcPatterns("tango", tg_patterns);
        cu_pool->setSrcPatterns("tango", tg_patterns);
#endif

#ifdef QUMBIA_EPICS_CONTROLS
        // do not allow host:20000/sys/tg_test/1/double_scalar
        // force at least one letter after ":"
        std::string ep_pattern = std::string("[A-Za-z0-9_]+:[A-Za-z_]+[A-Za-z_0-9]*");
        std::vector<std::string> ep_patterns;
        ep_patterns.push_back(ep_pattern);
        CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("epics", cuep);
        // m_ctrl_factory_pool  is in this example a private member of type CuControlsFactoryPool
        ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());   // register EPICS reader implementation
        ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());   // register EPICS writer implementation
        ctrl_factory_pool.setSrcPatterns("epics", ep_patterns);
        cu_pool->setSrcPatterns("epics", ep_patterns);
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

