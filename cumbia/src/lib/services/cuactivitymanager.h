#ifndef CUACTIVITYTRACKER_H
#define CUACTIVITYTRACKER_H

#include <cuservicei.h>
#include <vector>
#include <map>
#include <mutex>

class CuThreadInterface;
class CuActivity;
class CuData;
class CuThreadListener;

/** \brief This service stores the links between threads, activities and thread listeners.
 *
 * \par Note
 * The service must be created and its methods accessed from the main thread.
 */
class CuActivityManager : public CuServiceI
{
public:
    CuActivityManager();

    virtual ~CuActivityManager();

    void addConnection(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener);

    void removeConnection(CuActivity *a);

    void removeConnections(CuThreadInterface *t);

    void removeConnection(CuThreadListener *l);

    CuActivity *findActiveMatching(const CuData &token) ;

    CuThreadInterface *getThread(CuActivity *activity) ;

    bool isRegistered(const CuActivity *a);

    std::vector<CuActivity *> activitiesForThread(const CuThreadInterface *ti);

    int countActivitiesForThread(const CuThreadInterface *ti);

    std::vector<CuThreadListener *> getThreadListeners(const CuActivity *activity);

    bool connectionExists(CuThreadInterface *t, CuActivity *a, CuThreadListener *threadListener);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    std::multimap< CuThreadInterface *, CuActivity *> mConnectionsMultiMap;
    std::multimap<const CuActivity *, CuThreadListener *> mThreadListenersMultiMap;

    std::mutex m_mutex;
};

#endif // CUACTIVITYTRACKER_H
