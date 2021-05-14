#include "cuactionfactoryservice_impls.h"

#include <tango.h>
#include <map>
#include <shared_mutex>
#include <cumacros.h>

/*! @private
 */
class CuActionFactoryServiceImplBasePrivate {
public:
    std::list<CuTangoActionI * > actions;
};

/*! @private
 */
class CuActionFactoryServiceImpl_R_Private {
public:
    std::shared_mutex shared_mutex;
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


CuTangoActionI *CuActionFactoryServiceImpl_Base::registerAction(const string &src, const CuTangoActionFactoryI &f, CumbiaTango *ct) {
    CuTangoActionI* action = NULL;
    std::list<CuTangoActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it)
        if((*it)->getType() == f.getType() && (*it)->getSource().getName() == src /*&& !(*it)->exiting()*/ ) {
            break;
        }

    if(it == d->actions.end())
    {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    return action;
}

CuTangoActionI *CuActionFactoryServiceImpl_Base::find(const string &name, CuTangoActionI::Type at) {
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
    std::list<CuTangoActionI *>::iterator it;
    it = d->actions.begin();
    while( it != d->actions.end())
        ( (*it)->getType() == at && (*it)->getSource().getName() == src ) ? it = d->actions.erase(it) : ++it;
}

void CuActionFactoryServiceImpl_Base::cleanup() {
    std::list<CuTangoActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end()) {
        delete (*it);
        it = d->actions.erase(it);
    }
}

/*!
 *  non thread safe version
 */
CuActionFactoryServiceImpl::CuActionFactoryServiceImpl() {
    d = new CuActionFactoryServiceImplPrivate;
    d->creation_thread = pthread_self();
}

CuActionFactoryServiceImpl::~CuActionFactoryServiceImpl() {
    delete d;
}

CuTangoActionI *CuActionFactoryServiceImpl::registerAction(const string &src, const CuTangoActionFactoryI &f, CumbiaTango *ct) {
    assert(d->creation_thread == pthread_self());
    printf("\e[1;35mCuActionFactoryServiceImpl::registerAction [NON THREAD SAFE]\e[0m\n");
    return CuActionFactoryServiceImpl_Base::registerAction(src, f, ct);
}

CuTangoActionI *CuActionFactoryServiceImpl::find(const string &name, CuTangoActionI::Type at) {
    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::find(name, at);
}

size_t CuActionFactoryServiceImpl::count() const {
    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::count();
}

void CuActionFactoryServiceImpl::unregisterAction(const string &src, CuTangoActionI::Type at) {
    printf("\e[1;35mCuActionFactoryServiceImpl::unregisterAction [NON THREAD SAFE]\e[0m\n");
    assert(d->creation_thread == pthread_self());
    return CuActionFactoryServiceImpl_Base::unregisterAction(src, at);
}

void CuActionFactoryServiceImpl::cleanup() {
    assert(d->creation_thread == pthread_self());
    CuActionFactoryServiceImpl_Base::cleanup();
}


/*!
 *  thread safe version
 */
CuActionFactoryServiceImpl_TS::CuActionFactoryServiceImpl_TS() {
    d = new CuActionFactoryServiceImpl_R_Private;
}

CuActionFactoryServiceImpl_TS::~CuActionFactoryServiceImpl_TS() {
    delete d;
}

CuTangoActionI *CuActionFactoryServiceImpl_TS::registerAction(const string &src, const CuTangoActionFactoryI &f, CumbiaTango *ct) {
    printf("\e[1;32mCuActionFactoryServiceImpl::unregisterAction [ THREAD SAFE]\e[0m\n");
    std::unique_lock lock(d->shared_mutex);
    return CuActionFactoryServiceImpl_Base::registerAction(src, f, ct);
}

CuTangoActionI *CuActionFactoryServiceImpl_TS::find(const string &name, CuTangoActionI::Type at) {
    std::unique_lock lock(d->shared_mutex);
    return CuActionFactoryServiceImpl_Base::find(name, at);
}

size_t CuActionFactoryServiceImpl_TS::count() const {
    std::unique_lock lock(d->shared_mutex);
    return CuActionFactoryServiceImpl_Base::count();
}

void CuActionFactoryServiceImpl_TS::unregisterAction(const string &src, CuTangoActionI::Type at) {
    printf("\e[1;32mCuActionFactoryServiceImpl::unregisterAction [ THREAD SAFE]\e[0m\n");
    std::unique_lock lock(d->shared_mutex);
    return CuActionFactoryServiceImpl_Base::unregisterAction(src, at);
}

void CuActionFactoryServiceImpl_TS::cleanup() {
    std::unique_lock lock(d->shared_mutex);
    CuActionFactoryServiceImpl_Base::cleanup();
}
