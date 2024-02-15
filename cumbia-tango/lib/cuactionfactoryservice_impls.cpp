#include "cuactionfactoryservice_impls.h"

#include <tango.h>
#include <map>
#include <cumacros.h>
#include <unordered_map>
#include <assert.h>

/*! @private
 */
class CuActionFactoryServiceImplBasePrivate {
public:
    CuActionFactoryServiceImplBasePrivate() : reserve(0) {}
    // multimap: to the same src can be associated CuTangoActionI of different types
    std::unordered_multimap<std::string, CuTangoActionI * > actions;
    unsigned reserve;
    pthread_t creation_thread;
};

/*! @private
 */
class CuActionFactoryServiceImplPrivate {
public:
};

CuActionFactoryServiceImpl_Base::CuActionFactoryServiceImpl_Base() {
    d = new CuActionFactoryServiceImplBasePrivate;
    d->creation_thread = pthread_self();
}

CuActionFactoryServiceImpl_Base::~CuActionFactoryServiceImpl_Base() {
    delete d;
}

CuTangoActionI *CuActionFactoryServiceImpl_Base::registerAction(const TSource& src, const CuTangoActionFactoryI &f, CumbiaTango *ct, bool *isnew) {

    assert(pthread_self() == d->creation_thread);
    CuTangoActionI* action = nullptr;
    auto range = d->actions.equal_range(src.getName());
    for(auto it = range.first; action == nullptr && it != range.second; ++it) {
        if(it->second->getType() == f.getType()) {
            action = it->second;
        }
    }
    *isnew = (action == nullptr);
    if(*isnew) {
        std::string s = src.getName();
        action = f.create(src, ct);
        d->actions.insert(std::pair<std::string, CuTangoActionI *>{s, action});
    }
    return action;
}

CuTangoActionI *CuActionFactoryServiceImpl_Base::find(const string &name, CuTangoActionI::Type at) {
    assert(pthread_self() == d->creation_thread);
    auto range = d->actions.equal_range(name);
    for(auto it = range.first; it != range.second; ++it) {
        if(it->second->getType() == at /*&& !(*it)->exiting()*/ )
            return it->second;
    }
    return nullptr;
}

size_t CuActionFactoryServiceImpl_Base::count() const {
    assert(pthread_self() == d->creation_thread);
    return d->actions.size();
}

void CuActionFactoryServiceImpl_Base::unregisterAction(const TSource &src, CuTangoActionI::Type at) {
    assert(pthread_self() == d->creation_thread);
    auto range = d->actions.equal_range(src.getName());
    auto it = range.first;
    while(it != range.second) {
        if(it->second->getType() == at) {
            it = d->actions.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CuActionFactoryServiceImpl_Base::cleanup() {
    std::unordered_multimap<std::string, CuTangoActionI * >::iterator it = d->actions.begin();
    while(it != d->actions.end()) {
        delete (it->second);
        it = d->actions.erase(it);
    }
}

CuActionFactoryServiceImpl::CuActionFactoryServiceImpl() {
    d = new CuActionFactoryServiceImplPrivate;
}

CuActionFactoryServiceImpl::~CuActionFactoryServiceImpl() {
    pdelete("CuActionFactoryServiceImpl %p", this);
    delete d;
}

CuTangoActionI *CuActionFactoryServiceImpl::registerAction(const string &src, const CuTangoActionFactoryI &f, CumbiaTango *ct, bool *isnew) {
    return CuActionFactoryServiceImpl_Base::registerAction(src, f, ct, isnew);
}

CuTangoActionI *CuActionFactoryServiceImpl::find(const string &name, CuTangoActionI::Type at) {
    return CuActionFactoryServiceImpl_Base::find(name, at);
}

size_t CuActionFactoryServiceImpl::count() const {
    return CuActionFactoryServiceImpl_Base::count();
}

void CuActionFactoryServiceImpl::unregisterAction(const string &src, CuTangoActionI::Type at) {
    return CuActionFactoryServiceImpl_Base::unregisterAction(src, at);
}

void CuActionFactoryServiceImpl::cleanup() {
    CuActionFactoryServiceImpl_Base::cleanup();
}
