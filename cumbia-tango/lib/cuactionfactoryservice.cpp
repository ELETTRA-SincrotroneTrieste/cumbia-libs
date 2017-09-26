#include "cuactionfactoryservice.h"
#include "cutangoactionfactoryi.h"
#include "cutango-world.h"
#include "tdevice.h"
#include "tsource.h"
#include "cutangoactioni.h"

#include <culog.h>
#include <tango.h>
#include <map>
#include <mutex>
#include <cumacros.h>
class CuActionFactoryServicePrivate
{
public:
    std::list<CuTangoActionI * > actions;
    std::mutex mutex;
    std::string lastError;
    CuLog *log;
    CuLogImplI *logImpl;
};

CuActionFactoryService::CuActionFactoryService()
{
    d = new CuActionFactoryServicePrivate;
    d->logImpl = new CuConLogImpl;
    d->log = new CuLog(d->logImpl);
}

CuActionFactoryService::~CuActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete d->logImpl;
    delete d->log;
    delete d;
}

/*
 * same source, different CuTangoActionI type: two distinct actions.
 * Share actions of the same type among same sources.
 */
CuTangoActionI* CuActionFactoryService::registerAction(const std::string& src,
                                                       const CuTangoActionFactoryI& f,
                                                       CumbiaTango* ct)
{
    CuTangoActionI* action = NULL;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src)
            break;

    if(it == d->actions.end())
    {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    return action;
}

CuTangoActionI *CuActionFactoryService::findAction(const string &src, CuTangoActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src )
            return (*it);
    return NULL;
}

string CuActionFactoryService::getLastError() const
{
    return d->lastError;
}

void CuActionFactoryService::unregisterAction(const string &src, CuTangoActionI::Type at)
{
    printf("\e[1;31mCuActionFactoryService::unregisterAction for src %s and type %d\e[0m\n",
           src.c_str(), at);
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            it = d->actions.erase(it);
}

void CuActionFactoryService::deleteActions()
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        printf("CuActionFactoryService::deleteActions: deleting action %p (%s) type %d\n",
               (*it), (*it)->getSource().getName().c_str(), (*it)->getType());
        delete (*it);
        it = d->actions.erase(it);
    }
}

CuTangoActionI *CuActionFactoryService::findAction(const std::string& src, CuTangoActionI::Type at, CuDataListener *l)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            return (*it);
    return NULL;
}

std::string CuActionFactoryService::getName() const
{
    return std::string("DeviceFactory");
}

CuServices::Type CuActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuActionFactoryServiceType);
}
