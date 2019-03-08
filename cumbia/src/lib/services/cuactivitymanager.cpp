#include "cuactivitymanager.h"
#include "cudata.h"
#include "cuactivity.h"
#include "cumacros.h"

/** \brief The Cumbia Activity Manager keeps the links between threads, activities and thread listeners.
 *         CuActivityManager deals with adding, removing and finding objects connected with each other.
 *
 * \par Thread safety.
 * All the methods in this class are thread safe and can be accessed from within any CuActivity or
 * other thread.
 */
CuActivityManager::CuActivityManager()
{

}

CuActivityManager::~CuActivityManager()
{
    pdelete("~CuActivityManager %p", this);
    // std::lock_guard<std::mutex> lock(m_mutex);
    mConnectionsMultiMap.clear();
    mThreadListenersMultiMap.clear();
}

void CuActivityManager::addConnection(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    pbviolet2("CuActivityManager::addConnection: adding  tern %p %p %p  token \e[1;36m%s\e[0m",
              t, a, threadListener, a->getToken().toString().c_str());
    std::pair< CuThreadInterface *, CuActivity *> p(t, a);
    std::pair<CuActivity *, CuThreadListener *> pl(a, threadListener);
    mConnectionsMultiMap.insert(p);
    mThreadListenersMultiMap.insert(pl);
}

/** \brief Removes all links where a is involved
 *
 * @param a the activity to be removed together with the thread and thread listner(s) it's linked with
 *
 * This unbinds *threads from activities* and *thread listeners* (CuThreadListener)
 * from *activities* (CuActivity).
 *
 * See CuActivityManager::removeConnection
 */
void CuActivityManager::removeConnection(CuActivity *a)
{
    pviolet("CuActivityManager::removeConnection: removing connections for activity %p", a);
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it = mConnectionsMultiMap.begin();
    while(it != mConnectionsMultiMap.end())
    {
        if(it->second == a)
            it = mConnectionsMultiMap.erase(it);
        else
            ++it;
    }
    std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit = mThreadListenersMultiMap.begin();
    while(lit != mThreadListenersMultiMap.end())
    {
        if(lit->first == a)
            lit = mThreadListenersMultiMap.erase(lit);
        else
            ++lit;
    }

}

/** \brief Removes all links involving the CuThreadInterface passed as input
 *
 * @param t the CuThreadInterface thread input argument
 *
 * This method
 * 1. finds all the activities a linked to t;
 * 2. removes the thread listeners linked to a
 * 3. removes the links between a and t
 *
 * @see removeConnection
 */
void CuActivityManager::removeConnections(CuThreadInterface *t)
{
    pbviolet2("CuActivityManager::removeConnections: removing connections for thread %p", t);
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it;
    std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit;
    it = mConnectionsMultiMap.begin();
    while(it != mConnectionsMultiMap.end())  {
        if(it->first == t) {
            CuActivity *a = it->second;
            lit = mThreadListenersMultiMap.begin();
            while(lit != mThreadListenersMultiMap.end())
            {
                if(lit->first == a)
                    lit = mThreadListenersMultiMap.erase(lit);
                else
                    lit++;
            }
            it = mConnectionsMultiMap.erase(it);
        }
        else
            it++;
    }
}

void CuActivityManager::removeConnection(CuThreadListener *l)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit = mThreadListenersMultiMap.begin();
    while(lit != mThreadListenersMultiMap.end())
    {
        if(lit->second == l)
            lit = mThreadListenersMultiMap.erase(lit);
        else
            lit++;
    }
}

/*! \brief finds an activity matching the given token
 *
 * @return an activity matching the given token or NULL if either no activity matches the token
 *         or the found activity is disposable (it cannot be reused)
 *
 * \note If an activity matches the given token but is marked as disposable, NULL is returned.
 *
 * \par Disposable activities
 * Cumbia::unregisterActivity marks an activity disposable when it must be considered useless because
 * on its way to destruction.
 */
CuActivity *CuActivityManager::findActiveMatching(const CuData &token)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap< CuThreadInterface *, CuActivity *>::const_iterator it;
    for(it = mConnectionsMultiMap.begin(); it != mConnectionsMultiMap.end(); ++it)
    {
        const CuActivity *a = it->second;
        if(a->matches(token) && !a->isDisposable())
            return it->second;
    }
    return NULL;
}

/*! \brief returns the CuThreadInterface that executes the given activity
 *
 * @param activity a pointer to the CuActivity you want to get the thread
 *        it is run in
 * @return a pointer to the thread (CuThreadInterface) where the activity is
 *         run, or NULL if the activity is not found in the activity manager
 *         connections map
 */
CuThreadInterface *CuActivityManager::getThread(CuActivity *activity)
{
//    cuprintf("CuActivityManager.getThread: activity: %p this thread 0x%lx\n", activity, pthread_self());
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap< CuThreadInterface *, CuActivity *>::const_iterator it;
    for(it = mConnectionsMultiMap.begin(); it != mConnectionsMultiMap.end(); ++it)
    {
        if(it->second == activity)
            return it->first;
    }
    return NULL;
}

std::vector<CuThreadListener *> CuActivityManager::getThreadListeners(const CuActivity *activity)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::vector< CuThreadListener *> listeners;
    std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator lit = mThreadListenersMultiMap.find(activity);
    std::pair<std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator,
            std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator> ret;
    ret = mThreadListenersMultiMap.equal_range(activity);
    for(std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator it = ret.first; it != ret.second; ++it)
        listeners.push_back(lit->second);

    return listeners;
}

bool CuActivityManager::connectionExists(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener)
{
    pbviolet2("CuActivityManager::connectionExists: see if tern %p %p %p is already linked", t, a, threadListener);
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap< CuThreadInterface *, CuActivity *>::const_iterator it;
    for(it = mConnectionsMultiMap.begin(); it != mConnectionsMultiMap.end(); ++it)
    {
        if(it->second == a && it->first == t)
        {
            std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit;
            for(lit = mThreadListenersMultiMap.begin(); lit != mThreadListenersMultiMap.end(); ++lit)
                if(lit->first == a && lit->second == threadListener)
                    return true;
        }
    }
    return false;
}


std::vector<CuActivity *> CuActivityManager::activitiesForThread(const CuThreadInterface *ti)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<CuActivity *> v;
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it;
    for(it = mConnectionsMultiMap.begin(); it != mConnectionsMultiMap.end(); ++it)
    {
        if(it->first == ti)
            v.push_back(it->second);
    }
    pbviolet2("CuActivityManager::activitiesForThread: returning vector of activities for thread %p (there are %lu)", ti, v.size());
    return v;
}

int CuActivityManager::countActivitiesForThread(const CuThreadInterface *ti)
{
    int count = 0;
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it;
    for(it = mConnectionsMultiMap.begin(); it != mConnectionsMultiMap.end(); ++it)
        if(it->first == ti)
            count++;
    pbviolet2("CuActivityManager::countActivitiesForThread: returning vector of activities for thread %p (there are %d)", ti, count);
    return count;
}

std::string CuActivityManager::getName() const
{
    return std::string("CuActivityTracker");
}

CuServices::Type CuActivityManager::getType() const
{
    return CuServices::ActivityManager;
}
