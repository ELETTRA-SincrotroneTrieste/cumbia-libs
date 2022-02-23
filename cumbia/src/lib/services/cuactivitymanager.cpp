#include "cuactivitymanager.h"
#include "cudata.h"
#include "cuactivity.h"
#include "cumacros.h"
#include <map>
#include <mutex> // for unique_lock
#include <shared_mutex>

class CuActivityManagerPrivate {
public:
    std::multimap< CuThreadInterface *, CuActivity *> conn_mumap;
    std::multimap<const CuActivity *, CuThreadListener *> th_lis_mumap;
    std::shared_mutex shared_mu;
    pthread_t creation_thread;
};

/** \brief The Cumbia Activity Manager keeps the links between threads, activities and thread listeners.
 *         CuActivityManager deals with adding, removing and finding objects connected with each other.
 *
 * \par Thread safety.
 * All the methods in this class are thread safe and can be accessed from within any CuActivity or
 * other thread.
 */
CuActivityManager::CuActivityManager() {
    d = new CuActivityManagerPrivate;
    d->creation_thread = pthread_self();
}

CuActivityManager::~CuActivityManager()
{
    pdelete("~CuActivityManager %p", this);
    delete d;
}

void CuActivityManager::addConnection(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    std::pair< CuThreadInterface *, CuActivity *> p(t, a);
    std::pair<CuActivity *, CuThreadListener *> pl(a, threadListener);
    std::unique_lock lo(d->shared_mu);
    d->conn_mumap.insert(p);
    d->th_lis_mumap.insert(pl);
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
    std::unique_lock lo(d->shared_mu);
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it = d->conn_mumap.begin();
    while(it != d->conn_mumap.end())
    {
        if(it->second == a)
            it = d->conn_mumap.erase(it);
        else
            ++it;
    }
    std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit = d->th_lis_mumap.begin();
    while(lit != d->th_lis_mumap.end())
    {
        if(lit->first == a)
            lit = d->th_lis_mumap.erase(lit);
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
    std::unique_lock lo(d->shared_mu);
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it;
    std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit;
    it = d->conn_mumap.begin();
    while(it != d->conn_mumap.end())  {
        if(it->first == t) {
            CuActivity *a = it->second;
            lit = d->th_lis_mumap.begin();
            while(lit != d->th_lis_mumap.end())
            {
                if(lit->first == a)
                    lit = d->th_lis_mumap.erase(lit);
                else
                    lit++;
            }
            it = d->conn_mumap.erase(it);
        }
        else
            it++;
    }
}

void CuActivityManager::removeConnection(CuThreadListener *l)
{
    std::unique_lock lo(d->shared_mu);
    std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit = d->th_lis_mumap.begin();
    while(lit != d->th_lis_mumap.end())
    {
        if(lit->second == l)
            lit = d->th_lis_mumap.erase(lit);
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
CuActivity *CuActivityManager::find(const CuData &token)
{
    std::shared_lock lo(d->shared_mu);
    std::multimap< CuThreadInterface *, CuActivity *>::const_iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it) {
        const CuActivity *a = it->second;
        if(a->matches(token) && !a->isDisposable())
            return it->second;
    }
    return nullptr;
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
    std::shared_lock lo(d->shared_mu);
    std::multimap< CuThreadInterface *, CuActivity *>::const_iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it)
    {
        if(it->second == activity)
            return it->first;
    }
    return NULL;
}

std::vector<CuThreadListener *> CuActivityManager::getThreadListeners(const CuActivity *activity)
{
    std::shared_lock lo(d->shared_mu);
    std::vector< CuThreadListener *> listeners;
    std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator lit = d->th_lis_mumap.find(activity);
    std::pair<std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator,
            std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator> ret;
    ret = d->th_lis_mumap.equal_range(activity);
    for(std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator it = ret.first; it != ret.second; ++it)
        listeners.push_back(lit->second);

    return listeners;
}

bool CuActivityManager::connectionExists(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener)
{
    std::shared_lock lo(d->shared_mu);
    std::multimap< CuThreadInterface *, CuActivity *>::const_iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it)
    {
        if(it->second == a && it->first == t)
        {
            std::multimap<const CuActivity *, CuThreadListener *>::const_iterator lit;
            for(lit = d->th_lis_mumap.begin(); lit != d->th_lis_mumap.end(); ++lit)
                if(lit->first == a && lit->second == threadListener)
                    return true;
        }
    }
    return false;
}


std::vector<CuActivity *> CuActivityManager::activitiesForThread(const CuThreadInterface *ti)
{
    std::shared_lock lo(d->shared_mu);
    std::vector<CuActivity *> v;
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it)  {
        if(it->first == ti)
            v.push_back(it->second);
    }
    return v;
}

int CuActivityManager::countActivitiesForThread(const CuThreadInterface *ti)
{
    int count = 0;
    std::shared_lock lo(d->shared_mu);
    std::multimap< CuThreadInterface *, CuActivity *>::iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it)
        if(it->first == ti)
            count++;
    return count;
}

std::string CuActivityManager::getName() const {
    return std::string("CuActivityManager");
}

CuServices::Type CuActivityManager::getType() const {
    return CuServices::ActivityManager;
}
