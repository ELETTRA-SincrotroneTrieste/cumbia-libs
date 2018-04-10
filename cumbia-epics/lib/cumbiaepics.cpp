#include "cumbiaepics.h"
#include <cumbia.h>
#include <culog.h>
#include <cumacros.h>
#include <cudatalistener.h>

#include <cuserviceprovider.h>
#include "cuepactionfactoryservice.h"
#include "cuepactionfactoryi.h"
#include "cuepcaservice.h"
#include "cuepics-world.h"
#include "cumonitor.h"

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>

CumbiaEpics::CumbiaEpics()
{
    m_init();
}

CumbiaEpics::CumbiaEpics(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb)
{
    m_threadsEventBridgeFactory = teb;
    m_threadFactoryImplI = tfi;
    m_init();
}

void CumbiaEpics::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType), new CuActionFactoryService());
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType), new CuEpCAService());
}

void CumbiaEpics::setThreadFactoryImpl( CuThreadFactoryImplI *tfi)
{
    m_threadFactoryImplI = tfi;
}

void CumbiaEpics::setThreadEventsBridgeFactory( CuThreadsEventBridgeFactory_I *teb)
{
    m_threadsEventBridgeFactory = teb;
}

CumbiaEpics::~CumbiaEpics()
{
    pdelete("~CumbiaEpics %p\n", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(m_threadsEventBridgeFactory)
        delete m_threadsEventBridgeFactory;
    if(m_threadFactoryImplI)
        delete m_threadFactoryImplI;
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    af->deleteActions();
}

void CumbiaEpics::addAction(const std::__cxx11::string &source, CuDataListener *l, const CuEpicsActionFactoryI& f)
{
    CuEpicsWorld w;
    if(w.source_valid(source))
    {
        CuActionFactoryService *af =
                static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));

        CuEpicsActionI *a = af->findAction(source, f.getType());
        if(!a)
        {
            a = af->registerAction(source, f, this);
            a->start();
        }
        else
            printf("CumbiaEpics.addAction: action already found for source \"%s\" and type %d",
                  source.c_str(), f.getType());
        a->addDataListener(l);
    }
    else
        perr("CumbiaEpics.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

void CumbiaEpics::unlinkListener(const string &source, CuEpicsActionI::Type t, CuDataListener *l)
{
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuEpicsActionI* a = af->findAction(source, t);
    if(a)
        a->removeDataListener(l); /* when no more listeners, a stops itself */
}

CuEpicsActionI *CumbiaEpics::findAction(const std::string &source, CuEpicsActionI::Type t) const
{
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuEpicsActionI* a = af->findAction(source, t);
    return a;
}

CuThreadFactoryImplI *CumbiaEpics::getThreadFactoryImpl() const
{
    return m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaEpics::getThreadEventsBridgeFactory() const
{
    return m_threadsEventBridgeFactory;
}

int CumbiaEpics::getType() const
{
    return CumbiaEpicsType;
}



