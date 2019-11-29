#include "cumbiarandom.h"

#include <cumacros.h>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumbiarndworld.h>
#include <curndactionfactoryservice.h>
#include <curndactionfactoryi.h>
#include <QtDebug>

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>

CumbiaRandom::CumbiaRandom(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb)
{
    m_threadsEventBridgeFactory = teb;
    m_threadFactoryImplI = tfi;
    m_init();
}

CumbiaRandom::~CumbiaRandom()
{
    pdelete("~CumbiaRandom %p", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(m_threadsEventBridgeFactory)
        delete m_threadsEventBridgeFactory;
    if(m_threadFactoryImplI)
        delete m_threadFactoryImplI;
}

void CumbiaRandom::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType),
                                          new CuRNDActionFactoryService());

}

void CumbiaRandom::addAction(const std::string &source, CuDataListener *l, const CuRNDActionFactoryI &f)
{
    CumbiaRNDWorld w;
    if(w.source_valid(source))
    {
        CuRNDActionFactoryService *af =
                static_cast<CuRNDActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType)));

        CuRNDActionI *a = af->findActive(source, f.getType());
        if(!a) {
            a = af->registerAction(source, f, this);
            a->start();
        }
        a->addDataListener(l);
    }
    else
        perr("CumbiaRandom.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

void CumbiaRandom::unlinkListener(const string &source, CuRNDActionI::Type t, CuDataListener *l)
{
    CuRNDActionFactoryService *af =
            static_cast<CuRNDActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType)));
    std::vector<CuRNDActionI *> actions = af->find(source, t);
    for(size_t i = 0; i < actions.size(); i++) {
        actions[i]->removeDataListener(l); /* when no more listeners, a stops itself */
    }
}

CuRNDActionI *CumbiaRandom::findAction(const std::string &source, CuRNDActionI::Type t) const
{
    CuRNDActionFactoryService *af =
            static_cast<CuRNDActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType)));
    CuRNDActionI* a = af->findActive(source, t);
    return a;
}

CuThreadFactoryImplI *CumbiaRandom::getThreadFactoryImpl() const
{
     return m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaRandom::getThreadEventsBridgeFactory() const
{
    return m_threadsEventBridgeFactory;
}

int CumbiaRandom::getType() const
{
    return CumbiaRNDType;
}
