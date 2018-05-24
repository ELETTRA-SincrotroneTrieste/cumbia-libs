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

class CuEpicsActionFactoryServicePrivate
{
public:
    std::list<CuEpicsActionI * > actions;
    std::mutex mutex;
    std::string lastError;
    CuLog *log;
    CuLogImplI *logImpl;
};

CuEpicsActionFactoryService::CuEpicsActionFactoryService()
{
    d = new CuEpicsActionFactoryServicePrivate;
    d->logImpl = new CuConLogImpl;
    d->log = new CuLog(d->logImpl);
}

CuEpicsActionFactoryService::~CuEpicsActionFactoryService()
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
CuEpicsActionI* CuEpicsActionFactoryService::registerAction(const std::string& src,
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

std::vector<CuEpicsActionI *> CuEpicsActionFactoryService::find(const string &src, CuEpicsActionI::Type at)
{
    std::vector<CuEpicsActionI *> actions;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            actions.push_back (*it);
    return actions;
}

CuEpicsActionI *CuEpicsActionFactoryService::findActive(const string &src, CuEpicsActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src && !(*it)->exiting())
            return (*it);
    return NULL;
}

string CuEpicsActionFactoryService::getLastError() const
{
    return d->lastError;
}

void CuEpicsActionFactoryService::unregisterAction(const string &src, CuEpicsActionI::Type at)
{
    cuprintf("\e[1;31mCuActionFactoryService::unregisterAction for src %s and type %d\e[0m\n",
           src.c_str(), at);
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuEpicsActionI *>::iterator it;
    it = d->actions.begin();
    while( it != d->actions.end()) {
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            it = d->actions.erase(it);
        else
            it++;
    }
}

void CuEpicsActionFactoryService::deleteActions()
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


std::string CuEpicsActionFactoryService::getName() const
{
    return std::string("DeviceFactory");
}

CuServices::Type CuEpicsActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuActionFactoryServiceType);
}
