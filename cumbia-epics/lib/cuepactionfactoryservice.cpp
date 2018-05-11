#include "cuepactionfactoryi.h"
#include "cuepactionfactoryservice.h"
#include "cuepics-world.h"
#include "epsource.h"
#include "cuepactioni.h"

#include <culog.h>
#include <cadef.h>
#include <map>
#include <mutex>
#include <cumacros.h>

class CuActionFactoryServicePrivate
{
public:
    std::list<CuEpicsActionI * > actions;
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
 * same source, different CuEpicsActionI type: two distinct actions.
 * Share actions of the same type among same sources.
 */
CuEpicsActionI* CuActionFactoryService::registerAction(const std::string& src,
                                                       const CuEpicsActionFactoryI& f,
                                                       CumbiaEpics* ce)
{
    CuEpicsActionI* action = NULL;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src)
            break;

    if(it == d->actions.end())
    {
        action = f.create(src, ce);
        d->actions.push_back(action);
    }
    return action;
}

CuEpicsActionI *CuActionFactoryService::findAction(const string &src, CuEpicsActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            return (*it);
    return NULL;
}

string CuActionFactoryService::getLastError() const
{
    return d->lastError;
}

void CuActionFactoryService::unregisterAction(const string &src, CuEpicsActionI::Type at)
{
    cuprintf("\e[1;31mCuActionFactoryService::unregisterAction for src %s and type %d\e[0m\n",
           src.c_str(), at);
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at &&
                (*it)->getSource().getName() == src)
            it = d->actions.erase(it);
}

void CuActionFactoryService::deleteActions()
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        printf("CuActionFactoryService::deleteActions: deleting action %p (%s) type %d\n",
               (*it), (*it)->getSource().getName().c_str(), (*it)->getType());
        delete (*it);
        it = d->actions.erase(it);
    }
}

CuEpicsActionI *CuActionFactoryService::findAction(const std::string& src, CuEpicsActionI::Type at, CuDataListener *l)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
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
