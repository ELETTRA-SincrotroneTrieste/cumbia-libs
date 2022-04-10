#include "cuactivitymanager.h"
#include "cudata.h"
#include "cuactivity.h"
#include "cumacros.h"
#include <unordered_map>
#include <mutex> // for unique_lock
#include <shared_mutex>
#include <assert.h>

class CuActivityManagerPrivate {
public:
    std::unordered_multimap< CuActivity *, CuThreadInterface *> conn_mumap;
    std::unordered_multimap<const CuActivity *, CuThreadListener *> th_lis_mumap;
    pthread_t mythread;
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
    d->mythread = pthread_self();
}

CuActivityManager::~CuActivityManager() {
    assert(d->mythread == pthread_self());
    pdelete("~CuActivityManager %p", this);
    delete d;
}

void CuActivityManager::connect(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener) {
    assert(d->mythread == pthread_self());
    std::pair<CuActivity *, CuThreadInterface *> p(a, t);
    std::pair<CuActivity *, CuThreadListener *> pl(a, threadListener);
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
void CuActivityManager::disconnect(CuActivity *a) {
    assert(d->mythread == pthread_self());
    d->conn_mumap.erase(a);
    d->th_lis_mumap.erase(a);
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
void CuActivityManager::removeConnections(CuThreadInterface *t) {
    assert(d->mythread == pthread_self());
    std::unordered_multimap< CuActivity *, CuThreadInterface *>::iterator it;
    std::unordered_multimap<const CuActivity *, CuThreadListener *>::const_iterator lit;
    it = d->conn_mumap.begin();
    while(it != d->conn_mumap.end())  {
        if(it->second == t) {
            d->th_lis_mumap.erase(it->first);
            it = d->conn_mumap.erase(it);
        }
        else
            it++;
    }
}

void CuActivityManager::disconnect(CuThreadListener *l) {
    assert(d->mythread == pthread_self());
    std::unordered_multimap<const CuActivity *, CuThreadListener *>::iterator lit = d->th_lis_mumap.begin();
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
CuActivity *CuActivityManager::find(const CuData &token) {
    assert(d->mythread == pthread_self());
    std::unordered_multimap<CuActivity *, CuThreadInterface * >::const_iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it) {
        if(it->first->matches(token))
            return it->first;
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
CuThreadInterface *CuActivityManager::getThread(CuActivity *activity)  {
    assert(d->mythread == pthread_self());
    std::unordered_multimap<CuActivity *, CuThreadInterface * >::const_iterator it = d->conn_mumap.find(activity);
    return (it != d->conn_mumap.end()) ? it->second : nullptr;
}

std::vector<CuThreadListener *> CuActivityManager::getThreadListeners(const CuActivity *activity) {
    assert(d->mythread == pthread_self());
    std::vector< CuThreadListener *> listeners;
    std::unordered_multimap<const CuActivity *,  CuThreadListener *>::const_iterator lit = d->th_lis_mumap.find(activity);
    std::pair<std::unordered_multimap<const CuActivity *,  CuThreadListener *>::const_iterator,
            std::unordered_multimap<const CuActivity *,  CuThreadListener *>::const_iterator> ret;
    ret = d->th_lis_mumap.equal_range(activity);
    for(std::unordered_multimap<const CuActivity *,  CuThreadListener *>::const_iterator it = ret.first; it != ret.second; ++it)
        listeners.push_back(lit->second);

    return listeners;
}

bool CuActivityManager::connectionExists(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener) {
    assert(d->mythread == pthread_self());
    std::unordered_multimap<CuActivity *, CuThreadInterface *>::const_iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it) {
        if(it->first == a && it->second == t) {
            std::unordered_multimap<const CuActivity *, CuThreadListener *>::const_iterator lit;
            for(lit = d->th_lis_mumap.begin(); lit != d->th_lis_mumap.end(); ++lit)
                if(lit->first == a && lit->second == threadListener)
                    return true;
        }
    }
    return false;
}


std::vector<CuActivity *> CuActivityManager::activitiesForThread(const CuThreadInterface *ti) {
    assert(d->mythread == pthread_self());
    std::vector<CuActivity *> v;
    std::unordered_multimap<CuActivity *,  CuThreadInterface *>::iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it)  {
        if(it->second == ti)
            v.push_back(it->first);
    }
    return v;
}

int CuActivityManager::countActivitiesForThread(const CuThreadInterface *ti) {
    assert(d->mythread == pthread_self());
    int count = 0;
    std::unordered_multimap<CuActivity *,  CuThreadInterface *>::iterator it;
    for(it = d->conn_mumap.begin(); it != d->conn_mumap.end(); ++it)
        if(it->second == ti)
            count++;
    return count;
}

std::string CuActivityManager::getName() const {
    return std::string("CuActivityManager");
}

CuServices::Type CuActivityManager::getType() const {
    return CuServices::ActivityManager;
}
