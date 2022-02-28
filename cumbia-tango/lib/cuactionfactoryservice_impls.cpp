#include "cuactionfactoryservice_impls.h"

#include <tango.h>
#include <map>
#include <shared_mutex>
#include <mutex>
#include <cumacros.h>
#include <unordered_map>

#include <chrono>

/*! @private
 */
class CuActionFactoryServiceImplBasePrivate {
public:
    CuActionFactoryServiceImplBasePrivate() : reserve(0) {}
    // multimap: to the same src can be associated CuTangoActionI of different types
    std::unordered_multimap<std::string, CuTangoActionI * > actions;
    unsigned reserve;
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
    CuTangoActionI* action = nullptr;
    auto range = d->actions.equal_range(src);
    for(auto it = range.first; action == nullptr && it != range.second; ++it)
        if(it->second->getType() == f.getType() /*&& !(*it)->exiting()*/ ) {
            action = it->second;
        }
    *isnew = action == nullptr;
    if(*isnew) {
        action = f.create(src, ct);
        d->actions.insert(std::pair<std::string, CuTangoActionI *>{src, action});
    }
    return action;
}

CuTangoActionI *CuActionFactoryServiceImpl_Base::find(const string &name, CuTangoActionI::Type at) {
    auto range = d->actions.equal_range(name);
    for(auto it = range.first; it != range.second; ++it) {
        if(it->second->getType() == at /*&& !(*it)->exiting()*/ )
            return it->second;
    }
    return nullptr;
}

size_t CuActionFactoryServiceImpl_Base::count() const {
    return d->actions.size();
}

void CuActionFactoryServiceImpl_Base::unregisterAction(const string &src, CuTangoActionI::Type at) {
    std::unordered_multimap<std::string, CuTangoActionI * >::iterator it = d->actions.begin();
    while(it != d->actions.end()) {
        it->second->getSource().getName() == src && it->second->getType() == at ? it = d->actions.erase(it) : ++it;
    }
}

void CuActionFactoryServiceImpl_Base::cleanup() {
    std::unordered_multimap<std::string, CuTangoActionI * >::iterator it = d->actions.begin();
    while(it != d->actions.end()) {
        delete (it->second);
        it = d->actions.erase(it);
    }
}

void CuActionFactoryServiceImpl_Base::reserve(int chunks) {
    d->reserve = chunks;
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

void CuActionFactoryServiceImpl::reserve(int chunks) {
    CuActionFactoryServiceImpl_Base::reserve(chunks);
}
