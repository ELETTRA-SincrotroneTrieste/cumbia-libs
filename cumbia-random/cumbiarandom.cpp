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

CumbiaWebSocket::CumbiaWebSocket(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb)
{
    qDebug() << "CumbiaWebSocket constructor...\n";
    m_threadsEventBridgeFactory = teb;
    m_threadFactoryImplI = tfi;
    m_init();
}

CumbiaWebSocket::~CumbiaWebSocket()
{
    pdelete("~CumbiaWebSocket %p", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(m_threadsEventBridgeFactory)
        delete m_threadsEventBridgeFactory;
    if(m_threadFactoryImplI)
        delete m_threadFactoryImplI;
}

void CumbiaWebSocket::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType),
                                          new CuRNDActionFactoryService());

}

void CumbiaWebSocket::addAction(const std::string &source, CuDataListener *l, const CuRNDActionFactoryI &f)
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
        else {
            cuprintf("CumbiaWebSocket.addAction: action %p already found for source \"%s\" and type %d thread 0x%lx TYPE %d\n",
                  a, source.c_str(), f.getType(), pthread_self(), f.getType());
        }
        a->addDataListener(l);
    }
    else
        perr("CumbiaWebSocket.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

void CumbiaWebSocket::unlinkListener(const string &source, CuRNDActionI::Type t, CuDataListener *l)
{
    CuRNDActionFactoryService *af =
            static_cast<CuRNDActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType)));
    std::vector<CuRNDActionI *> actions = af->find(source, t);
    for(size_t i = 0; i < actions.size(); i++) {
        actions[i]->removeDataListener(l); /* when no more listeners, a stops itself */
    }
}

CuRNDActionI *CumbiaWebSocket::findAction(const std::string &source, CuRNDActionI::Type t) const
{
    CuRNDActionFactoryService *af =
            static_cast<CuRNDActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuRNDActionFactoryService::CuRNDActionFactoryServiceType)));
    CuRNDActionI* a = af->findActive(source, t);
    return a;
}

CuThreadFactoryImplI *CumbiaWebSocket::getThreadFactoryImpl() const
{
     return m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaWebSocket::getThreadEventsBridgeFactory() const
{
    return m_threadsEventBridgeFactory;
}

int CumbiaWebSocket::getType() const
{
    return CumbiaRNDType;
}
