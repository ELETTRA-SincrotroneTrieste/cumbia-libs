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

/** Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuTangoActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *  \li If an action with the same source and type exists, but is stopping, it is discarded and a new
 *      action is created.
 *
 * @see findAction
 * @see unregisterAction
 */
CuTangoActionI* CuActionFactoryService::registerAction(const std::string& src,
                                                       const CuTangoActionFactoryI& f,
                                                       CumbiaTango* ct)
{
    CuTangoActionI* action = NULL;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src && !(*it)->exiting())
            break;

    if(it == d->actions.end())
    {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    return action;
}

CuTangoActionI *CuActionFactoryService::findActive(const string &src, CuTangoActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src && !(*it)->exiting())
            return (*it);
    return NULL;
}

CuTangoActionI *CuActionFactoryService::find(const string &src, CuTangoActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            return (*it);
    return NULL;
}

size_t CuActionFactoryService::count() const
{
    return d->actions.size();
}

string CuActionFactoryService::getLastError() const
{
    return d->lastError;
}

/** \brief Unregister the action with the given source and type if the action's exiting() method returns true
 *
 */
void CuActionFactoryService::unregisterAction(const string &src, CuTangoActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it;
    size_t siz = d->actions.size();
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src && (*it)->exiting())
            it = d->actions.erase(it);
    if(d->actions.size() == siz) // nothing removed
        perr("CuActionFactoryService::unregisterAction: no actions unregistered");
}

void CuActionFactoryService::cleanup()
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTangoActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        delete (*it);
        it = d->actions.erase(it);
    }
}

//CuTangoActionI *CuActionFactoryService::findAction(const std::string& src, CuTangoActionI::Type at, CuDataListener *l)
//{
//    std::lock_guard<std::mutex> lock(d->mutex);
//    std::list<CuTangoActionI *>::iterator it;
//    for(it = d->actions.begin(); it != d->actions.end(); ++it)
//        if((*it)->getType() == at && (*it)->getSource().getName() == src)
//            return (*it);
//    return NULL;
//}

std::string CuActionFactoryService::getName() const
{
    return std::string("DeviceFactory");
}

CuServices::Type CuActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuActionFactoryServiceType);
}
