#include "cuactionfactoryservice_impls.h"

#include <tango.h>
#include <map>
#include <shared_mutex>
#include <mutex>
#include <cumacros.h>
#include <unordered_map>

#include <chrono>

unsigned long tottime = 0;

/*! @private
 */
class CuActionFactoryServiceImplBasePrivate {
public:
    CuActionFactoryServiceImplBasePrivate() {}
    std::list<CuTangoActionI * > actions;
};

/*! @private
 */
class CuActionFactoryServiceImplPrivate {
public:
    pthread_t creation_thread;
};

CuActionFactoryServiceImpl_Base::CuActionFactoryServiceImpl_Base() {
    d = new CuActionFactoryServiceImplBasePrivate;
}

CuActionFactoryServiceImpl_Base::~CuActionFactoryServiceImpl_Base() {
    delete d;
}

CuTangoActionI *CuActionFactoryServiceImpl_Base::registerAction(const string &src, const CuTangoActionFactoryI &f, CumbiaTango *ct, bool *isnew) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    CuTangoActionI* action = nullptr;
    std::list<CuTangoActionI *>::const_iterator it;
    for(it = d->actions.begin(); action == nullptr && it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src /*&& !(*it)->exiting()*/ )
            action = *it;
    *isnew = action == nullptr;
    if(*isnew) {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    tottime += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    printf("CuActionFactoryServiceImpl_Base::registerActio total time spent inCuActionFactoryServiceImpl_Base restigere/unregierer %luus\n",
           tottime);
    return action;
}

CuTangoActionI *CuActionFactoryServiceImpl_Base::find(const string &name, CuTangoActionI::Type at) {    
    assert(true);
    std::list<CuTangoActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it) {
        if((*it)->getType() == at && (*it)->getSource().getName() == name/* && !(*it)->exiting()*/)
            return (*it);
    }
    return nullptr;
}

size_t CuActionFactoryServiceImpl_Base::count() const {
    return d->actions.size();
}

void CuActionFactoryServiceImpl_Base::unregisterAction(const string &src, CuTangoActionI::Type at) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::list<CuTangoActionI *>::iterator it;
       it = d->actions.begin();
       while( it != d->actions.end())
           ( (*it)->getType() == at && (*it)->getSource().getName() == src ) ? it = d->actions.erase(it) : ++it;

   std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
   tottime += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

void CuActionFactoryServiceImpl_Base::cleanup() {
    std::list<CuTangoActionI *>::iterator it = d->actions.begin();
       while(it != d->actions.end()) {
           delete (*it);
           it = d->actions.erase(it);
       }
}

CuActionFactoryServiceImpl::CuActionFactoryServiceImpl() {
    d = new CuActionFactoryServiceImplPrivate;
    d->creation_thread = pthread_self();
}

CuActionFactoryServiceImpl::~CuActionFactoryServiceImpl() {
    delete d;
}

CuTangoActionI *CuActionFactoryServiceImpl::registerAction(const string &src, const CuTangoActionFactoryI &f, CumbiaTango *ct, bool *isnew) {
    //    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::registerAction(src, f, ct, isnew);
}

CuTangoActionI *CuActionFactoryServiceImpl::find(const string &name, CuTangoActionI::Type at) {
    //    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::find(name, at);
}

size_t CuActionFactoryServiceImpl::count() const {
    //    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::count();
}

void CuActionFactoryServiceImpl::unregisterAction(const string &src, CuTangoActionI::Type at) {
    //    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::unregisterAction(src, at);
}

void CuActionFactoryServiceImpl::cleanup() {
    //    assert(d->creation_thread == pthread_self());
    CuActionFactoryServiceImpl_Base::cleanup();
}

