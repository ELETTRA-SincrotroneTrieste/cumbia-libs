#include "cuactionfactoryservice.h"
#include "cutangoactionfactoryi.h"
#include "cutango-world.h"
#include "tdevice.h"
#include "tsource.h"
#include "cutangoactioni.h"

#include <tango.h>
#include <map>
#include <shared_mutex>
#include <cumacros.h>

/*! @private
 */
class CuActionFactoryServicePrivate
{
public:
    std::list<CuTangoActionI * > actions;
//  std::shared_mutex shared_mutex;
    pthread_t creation_thread;
};

/*! \brief the class constructor
 *
 */
CuActionFactoryService::CuActionFactoryService()
{
    d = new CuActionFactoryServicePrivate;
    d->creation_thread = pthread_self();
}

CuActionFactoryService::~CuActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete d;
}

/** \brief Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuTangoActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *
 * @see findAction
 * @see unregisterAction
 */
CuTangoActionI* CuActionFactoryService::registerAction(const std::string& src,
                                                       const CuTangoActionFactoryI& f,
                                                       CumbiaTango* ct)
{
    assert(d->creation_thread == pthread_self());
    CuTangoActionI* action = NULL;
//    std::unique_lock lock(d->shared_mutex);
    std::list<CuTangoActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src /*&& !(*it)->exiting()*/ ) {
            break;
        }

    if(it == d->actions.end())
    {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    return action;
}

/*! \brief find an action given the source name and the action type
 *
 * @param src the complete source (target) name (no wildcard)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 * @return the action with the given src and type.
 */
CuTangoActionI *CuActionFactoryService::find(const string &src, CuTangoActionI::Type at) {
    assert(d->creation_thread == pthread_self());
//    std::shared_lock lock(d->shared_mutex);
    std::list<CuTangoActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it) {
        if((*it)->getType() == at && (*it)->getSource().getName() == src/* && !(*it)->exiting()*/)
            return (*it);
    }
    return NULL;
}

/*! \brief return the number of registered actions
 *
 * @return the number of registered actions.
 *
 * Actions are added with registerAction and removed with unregisterAction
 */
size_t CuActionFactoryService::count() const {
    return d->actions.size();
}

/** \brief Unregister the action with the given source and type
 *
 * @param src the complete source name (no wildcards)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 *
 * \note
 * The action is <strong>not deleted</strong>.
 * CuTReader and CuTWriter auto delete themselves
 */
void CuActionFactoryService::unregisterAction(const string &src, CuTangoActionI::Type at) {
    assert(d->creation_thread == pthread_self());
//    std::unique_lock lock(d->shared_mutex);
    std::list<CuTangoActionI *>::iterator it;
    it = d->actions.begin();
    while( it != d->actions.end())
        ( (*it)->getType() == at && (*it)->getSource().getName() == src ) ? it = d->actions.erase(it) : ++it;
}

/*! \brief all registered actions are <strong>deleted</strong> and removed from the service internal list.
 *
 * All references to the registered actions are removed from the service
 *
 * \note
 * The action is <strong>deleted</strong>.
 */
void CuActionFactoryService::cleanup() {
    assert(d->creation_thread == pthread_self());
//    std::unique_lock lock(d->shared_mutex);
    std::list<CuTangoActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end()) {
        delete (*it);
        it = d->actions.erase(it);
    }
}

/*! \brief returns the service name
 *
 * @return the constant string "*ActionFactoryService" identifying this type of service.
 *
 * Implements CuTangoActionI::getName pure virtual method
 */
std::string CuActionFactoryService::getName() const {
    return std::string("DeviceFactory");
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuActionFactoryServiceType = CuServices::User + 2)
 *
 * @see Type
 *
 * Implements CuTangoActionI::getType pure virtual method
 */
CuServices::Type CuActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuActionFactoryServiceType);
}
