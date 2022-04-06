#include "cutthread.h"
#include "cudevicefactoryservice.h"
#include "cutthreaddevices.h"

CuThreadInterface *CuTThreadFactoryImpl::createThread(const std::string &thread_token ,
                                                     CuThreadsEventBridge_I *eventsBridge,
                                                     const CuServiceProvider *service_provider,
                                                     std::vector<CuThreadInterface *> *threads) const {
    pretty_pri("new thread %s", thread_token.c_str());
    return new CuTThread(thread_token, eventsBridge, service_provider, threads);
}

class CuTThreadPrivate {
public:
    CuTThreadPrivate() : df(new CuTThreadDevices()) {
    }

    ~CuTThreadPrivate() {
        delete df;
    }

    CuDeviceFactory_I *df;
};

CuTThread::CuTThread(const std::string &token, CuThreadsEventBridge_I *threadEventsBridge, const CuServiceProvider *sp, std::vector<CuThreadInterface *> *thv_p)
 : CuThread(token, threadEventsBridge, sp, thv_p) {
    d = new CuTThreadPrivate;
}

CuTThread::~CuTThread() {
    delete d;
}

int CuTThread::type() const {
    return CuTThreadType;
}

CuDeviceFactory_I *CuTThread::device_factory() const {
    return d->df;
}
