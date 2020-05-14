#include "cuhttpactionfactoryservice.h"
#include "cuhttpactionfactoryi.h"
#include "cuhttp_source.h"
#include "cuhttpactiona.h"
#include <map>
#include <mutex>
#include <cumacros.h>
#include <QNetworkAccessManager>

/*! @private
 */
class CuHTTPActionFactoryServicePrivate
{
public:
    std::list<CuHTTPActionA * > actions;
    std::mutex mutex;
};

/*! \brief the class constructor
 *
 */
CuHTTPActionFactoryService::CuHTTPActionFactoryService()
{
    d = new CuHTTPActionFactoryServicePrivate;
}

CuHTTPActionFactoryService::~CuHTTPActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete d;
}

/** \brief Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuHTTPActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *  \li If an action with the same source and type exists, but is stopping, it is discarded and a new
 *      action is created.
 *
 * @see findAction
 * @see unregisterAction
 */
CuHTTPActionA *CuHTTPActionFactoryService::registerAction(const std::string& src,
                                                       const CuHTTPActionFactoryI &f,
                                                       QNetworkAccessManager* qnam,
                                                      const QString& url)
{
    CuHTTPActionA* action = NULL;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuHTTPActionA *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getFullName() == src && !(*it)->exiting()) {
            break;
        }

    if(it == d->actions.end())
    {
        action = f.create(src, qnam, url);
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
 * An action is *active* as long as CuHTTPActionI::exiting returns false.
 * Usually, an action is marked as *exiting* when CuHTTPActionI::stop is called.
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
CuHTTPActionA *CuHTTPActionFactoryService::findActive(const string &src, CuHTTPActionA::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuHTTPActionA *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getFullName() == src && !(*it)->exiting())
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
std::vector<CuHTTPActionA *> CuHTTPActionFactoryService::find(const string &src, CuHTTPActionA::Type at)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::vector <CuHTTPActionA *> actions;
    std::list<CuHTTPActionA *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == at && (*it)->getSource().getFullName() == src)
            actions.push_back(*it);
    return actions;
}

/*! \brief return the number of registered actions
 *
 * @return the number of registered actions.
 *
 * Actions are added with registerAction and removed with unregisterAction
 */
size_t CuHTTPActionFactoryService::count() const
{
    return d->actions.size();
}

/** \brief Unregister the action with the given source and type if the action's exiting() method returns true
 *
 * @param src the complete source name (no wildcards)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 *
 * if an action with the given source (target) and type is found and the action is
 * exiting (CuHTTPActionI::exiting returns false), it is removed
 * from the internal list of actions.
 *
 * If CuHTTPActionI::exiting returns false, the action is not removed.
 *
 * \note
 * The action is <strong>not deleted</strong>.
 *
 * @return the unregistered action or nullptr
 *
 */
CuHTTPActionA *CuHTTPActionFactoryService::unregisterAction(const std::string &src, CuHTTPActionA::Type at)
{
    CuHTTPActionA *a = nullptr;
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuHTTPActionA *>::iterator it;
    size_t siz = d->actions.size();

    it = d->actions.begin();
    while( it != d->actions.end())  {
        if((*it)->getType() == at && (*it)->getSource().getFullName() == src && (*it)->exiting()) {
            if(!a) a = (*it);
            it = d->actions.erase(it);
        }
        else
            ++it;
    }
    if(d->actions.size() == siz) // nothing removed
        perr("CuActionFactoryService::unregisterAction: no actions unregistered");
    return a;
}

/*! \brief all registered actions are <strong>deleted</strong> and removed from the service internal list.
 *
 * All references to the registered actions are removed from the service internal list,
 * whether or not CuHTTPActionI::exiting returns true
 *
 * \note
 * The action is <strong>deleted</strong>.
 */
void CuHTTPActionFactoryService::cleanup()
{
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuHTTPActionA *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        delete (*it);
        it = d->actions.erase(it);
    }
}

/*! \brief returns the service name
 *
 * @return the constant string "*CuHTTPActionFactoryService" identifying this type of service.
 *
 * Implements CuServiceI::getFullName pure virtual method
 */
std::string CuHTTPActionFactoryService::getName() const
{
    return std::string("CuHTTPActionFactoryService");
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuHTTPActionFactoryServiceType = CuServices::User + 25)
 *
 * @see Type
 *
 * Implements CuHTTPActionI::getType pure virtual method
 */
CuServices::Type CuHTTPActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuHTTPActionFactoryServiceType);
}
