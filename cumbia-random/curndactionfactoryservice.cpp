#include "curndactionfactoryservice.h"
#include "curndactionfactoryi.h"
#include "rnd_source.h"
#include "curndactioni.h"
#include <map>
#include <mutex>
#include <cumacros.h>

/*! @private
 */
class CuRNDActionFactoryServicePrivate
{
public:
    std::list<CuRNDActionI * > actions;
    std::mutex mutex;
};

/*! \brief the class constructor
 *
 */
CuRNDActionFactoryService::CuRNDActionFactoryService()
{
    d = new CuRNDActionFactoryServicePrivate;
}

CuRNDActionFactoryService::~CuRNDActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete d;
}

/** \brief Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuRNDActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *  \li If an action with the same source and type exists, but is stopping, it is discarded and a new
 *      action is created.
 *
 * @see findAction
 * @see unregisterAction
 */
CuRNDActionI *CuRNDActionFactoryService::registerAction(const std::string& src,
                                                       const CuRNDActionFactoryI &f,
                                                       CumbiaWebSocket* ct)
{
    CuRNDActionI* action = NULL;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuRNDActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src && !(*it)->exiting()) {
            break;
        }

    if(it == d->actions.end())
    {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    return action;
}

/*! \brief find an *active* action given the source name and the action type
 *
 * @param src the complete source (target) name (no wildcard)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 * @return the active action with the given src and type.
 *
 * \note
 * An action is *active* as long as CuRNDActionI::exiting returns false.
 * Usually, an action is marked as *exiting* when CuRNDActionI::stop is called.
 * For example, see CuTReader::stop and CuTWriter::stop
 * An action can be stopped when a data listener is removed, as in CuTReader::removeDataListener
 * and CuTWriter::removeDataListener.
 * CuTReader::removeDataListener and CuTWriter::removeDataListener are invoked by
 * CumbiaTango::unlinkListener. qumbia-tango-controls CuTControlsReader
 * (CuTControlsWriter) and qumbia-epics-controls CuEpControlsReader (CuEpControlsWriter) call
 * CumbiaTango::unlinkListener from within unsetSource (unsetTarget for writers).
 *
 * This sequence is an example if you ever need to deal with stopping and unregistering an
 * action from a client.
 *
 */
CuRNDActionI *CuRNDActionFactoryService::findActive(const string &src, CuRNDActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuRNDActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src && !(*it)->exiting())
            return (*it);
    return NULL;
}

/*! \brief find an action given its source and type
 *
 * @param src the complete source (target) name (no wildcard)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 * @return a list of actions with the given src and type.
 *
 * \par Note
 * Multiple actions can be found with the given src and type. This is likely to happen when
 * a new action is registered, stopped and registered again with the same src in a short time.
 * In this case, we end up with a not active action and an active one with the same src.
 *
 * \par Note
 * To find active actions (whose stop method hasn't been called yet), use findActive
 */
std::vector<CuRNDActionI *> CuRNDActionFactoryService::find(const string &src, CuRNDActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::vector <CuRNDActionI *> actions;
    std::list<CuRNDActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getName() == src)
            actions.push_back(*it);
    return actions;
}

/*! \brief return the number of registered actions
 *
 * @return the number of registered actions.
 *
 * Actions are added with registerAction and removed with unregisterAction
 */
size_t CuRNDActionFactoryService::count() const
{
    return d->actions.size();
}

/** \brief Unregister the action with the given source and type if the action's exiting() method returns true
 *
 * @param src the complete source name (no wildcards)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 *
 * if an action with the given source (target) and type is found and the action is
 * exiting (CuRNDActionI::exiting returns false), it is removed
 * from the internal list of actions.
 *
 * If CuRNDActionI::exiting returns false, the action is not removed.
 *
 * \note
 * The action is <strong>not deleted</strong>.
 * CuTReader and CuTWriter auto delete themselves from within CuTReader::onResult and CuTWriter::onResult
 * when the *exiting* flag is true.
 *
 */
void CuRNDActionFactoryService::unregisterAction(const std::string &src, CuRNDActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuRNDActionI *>::iterator it;
    size_t siz = d->actions.size();

    it = d->actions.begin();
    while( it != d->actions.end())  {
        if((*it)->getType() == at && (*it)->getSource().getName() == src && (*it)->exiting()) {
            it = d->actions.erase(it);
        }
        else
            ++it;
    }
    if(d->actions.size() == siz) // nothing removed
        perr("CuActionFactoryService::unregisterAction: no actions unregistered");
}

/*! \brief all registered actions are <strong>deleted</strong> and removed from the service internal list.
 *
 * All references to the registered actions are removed from the service internal list,
 * whether or not CuRNDActionI::exiting returns true
 *
 * \note
 * The action is <strong>deleted</strong>.
 */
void CuRNDActionFactoryService::cleanup()
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuRNDActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        delete (*it);
        it = d->actions.erase(it);
    }
}

/*! \brief returns the service name
 *
 * @return the constant string "*CuRNDActionFactoryService" identifying this type of service.
 *
 * Implements CuServiceI::getName pure virtual method
 */
std::string CuRNDActionFactoryService::getName() const
{
    return std::string("CuRNDActionFactoryService");
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuRNDActionFactoryServiceType = CuServices::User + 25)
 *
 * @see Type
 *
 * Implements CuRNDActionI::getType pure virtual method
 */
CuServices::Type CuRNDActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuRNDActionFactoryServiceType);
}
