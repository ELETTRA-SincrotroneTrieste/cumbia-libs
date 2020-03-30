#include "cuwsactionfactoryservice.h"
#include "cuwsactionfactoryi.h"
#include "ws_source.h"
#include "cuwsactioni.h"
#include <map>
#include <mutex>
#include <cumacros.h>

/*! @private
 */
class CuWSActionFactoryServicePrivate
{
public:
    std::list<CuWSActionI * > actions;
    std::mutex mutex;
};

/*! \brief the class constructor
 *
 */
CuWSActionFactoryService::CuWSActionFactoryService()
{
    d = new CuWSActionFactoryServicePrivate;
}

CuWSActionFactoryService::~CuWSActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete d;
}

/** \brief Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuWSActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *  \li If an action with the same source and type exists, but is stopping, it is discarded and a new
 *      action is created.
 *
 * @see findAction
 * @see unregisterAction
 */
CuWSActionI *CuWSActionFactoryService::registerAction(const std::string& src,
                                                       const CuWSActionFactoryI &f,
                                                       CuWSClient* cli,
                                                      const QString& http_url)
{
    CuWSActionI* action = NULL;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuWSActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src && !(*it)->exiting()) {
            break;
        }

    if(it == d->actions.end())
    {
        action = f.create(src, cli, http_url);
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
 * An action is *active* as long as CuWSActionI::exiting returns false.
 * Usually, an action is marked as *exiting* when CuWSActionI::stop is called.
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
CuWSActionI *CuWSActionFactoryService::findActive(const string &src, CuWSActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuWSActionI *>::const_iterator it;
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
std::vector<CuWSActionI *> CuWSActionFactoryService::find(const string &src, CuWSActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::vector <CuWSActionI *> actions;
    std::list<CuWSActionI *>::const_iterator it;
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
size_t CuWSActionFactoryService::count() const
{
    return d->actions.size();
}

/** \brief Unregister the action with the given source and type if the action's exiting() method returns true
 *
 * @param src the complete source name (no wildcards)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 *
 * if an action with the given source (target) and type is found and the action is
 * exiting (CuWSActionI::exiting returns false), it is removed
 * from the internal list of actions.
 *
 * If CuWSActionI::exiting returns false, the action is not removed.
 *
 * \note
 * The action is <strong>not deleted</strong>.
 * CuTReader and CuTWriter auto delete themselves from within CuTReader::onResult and CuTWriter::onResult
 * when the *exiting* flag is true.
 *
 */
void CuWSActionFactoryService::unregisterAction(const std::string &src, CuWSActionI::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuWSActionI *>::iterator it;
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
 * whether or not CuWSActionI::exiting returns true
 *
 * \note
 * The action is <strong>deleted</strong>.
 */
void CuWSActionFactoryService::cleanup()
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuWSActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        delete (*it);
        it = d->actions.erase(it);
    }
}

/*! \brief returns the service name
 *
 * @return the constant string "*CuWSActionFactoryService" identifying this type of service.
 *
 * Implements CuServiceI::getName pure virtual method
 */
std::string CuWSActionFactoryService::getName() const
{
    return std::string("CuWSActionFactoryService");
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuWSActionFactoryServiceType = CuServices::User + 25)
 *
 * @see Type
 *
 * Implements CuWSActionI::getType pure virtual method
 */
CuServices::Type CuWSActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuWSActionFactoryServiceType);
}
