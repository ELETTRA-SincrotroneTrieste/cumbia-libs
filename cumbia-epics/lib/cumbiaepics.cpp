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
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuEpicsActionFactoryService::CuActionFactoryServiceType), new CuEpicsActionFactoryService());
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
    CuEpicsActionFactoryService *af =
            static_cast<CuEpicsActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuEpicsActionFactoryService::CuActionFactoryServiceType)));
    af->deleteActions();
}

/*! \brief Creates and registers a new action if an active one with the given source and type is not found
 *
 * @return true an active action with the given source and type has been found and will be used
 * @return false a new action for the given source and type has been created and registered
 */
bool CumbiaEpics::addAction(const std::string &source, CuDataListener *l, const CuEpicsActionFactoryI& f)
{
    bool found = false;
    CuEpicsWorld w;
    if(w.source_valid(source))
    {
        CuEpicsActionFactoryService *af =
                static_cast<CuEpicsActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuEpicsActionFactoryService::CuActionFactoryServiceType)));

        CuEpicsActionI *a = af->findActive(source, f.getType());
        found = (a != NULL);
        if(!a)
        {
            a = af->registerAction(source, f, this);
            a->start();
        }
        else {
            printf("CumbiaEpics.addAction: action already found for source \"%s\" and type %d\n",
                  source.c_str(), f.getType());
        }
        a->addDataListener(l);
    }
    else
        perr("CumbiaEpics.addAction: source \"%s\" is not valid, ignoring", source.c_str());

    return found;
}

void CumbiaEpics::unlinkListener(const string &source, CuEpicsActionI::Type t, CuDataListener *l)
{
    CuEpicsActionFactoryService *af =
            static_cast<CuEpicsActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuEpicsActionFactoryService::CuActionFactoryServiceType)));
    std::vector<CuEpicsActionI *> actions = af->find(source, t);
    for(size_t i = 0; i < actions.size(); i++)
        actions[i]->removeDataListener(l); /* when no more listeners, a stops itself */
}

CuEpicsActionI *CumbiaEpics::findAction(const std::string &source, CuEpicsActionI::Type t) const
{
    CuEpicsActionFactoryService *af =
            static_cast<CuEpicsActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuEpicsActionFactoryService::CuActionFactoryServiceType)));
    CuEpicsActionI* a = af->findActive(source, t);
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



