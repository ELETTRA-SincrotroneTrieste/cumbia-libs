#include "cutthreaddevices.h"
#include "tdevice.h"

#include <assert.h>
#include <pthread.h>
#include <unordered_map>

class TThDevData {
public:
    TThDevData(TDevice *td, const std::string& tk) : tdevice(td), thread_token(tk) { }
    TDevice *tdevice;
    const std::string thread_token;
};

class CuTThreadDevicesPrivate {
public:
    CuTThreadDevicesPrivate() : mythread(0) {}
    ~CuTThreadDevicesPrivate() {
        int refcnt = -1;
        for(std::unordered_map<std::string, TThDevData>::iterator it = m_devmap.begin(); it != m_devmap.end(); ++it) {
            refcnt = it->second.tdevice->removeRef();
            if(refcnt == 0)
                delete it->second.tdevice;
        }
    }

    std::unordered_multimap<std::string, TThDevData> m_devmap;
    pthread_t mythread;
};


CuTThreadDevices::CuTThreadDevices() {
    d = new CuTThreadDevicesPrivate;
}

CuTThreadDevices::~CuTThreadDevices() {
    delete d;
}


TDevice *CuTThreadDevices::getDevice(const std::string &name, const std::string &thread_tok) {
    if(d->mythread == 0)
        d->mythread = pthread_self();
    assert(d->mythread == pthread_self());
    TDevice *td = nullptr;
    std::pair<std::unordered_multimap <std::string, TThDevData>::const_iterator, std::unordered_multimap <std::string, TThDevData>::iterator > ret;
    ret = d->m_devmap.equal_range(name);
    for(std::unordered_multimap<std::string, TThDevData>::const_iterator it = ret.first; !td && it != ret.second; ++it) {
        if(it->second.thread_token == thread_tok)
            td = it->second.tdevice;
    }

    if(!td) {
        td = new TDevice(name);
        TThDevData devd(td, thread_tok);
        std::pair<std::string, TThDevData > p(name, devd);
        d->m_devmap.insert(p);
    }
    td->addRef();
    return td;
}

int CuTThreadDevices::removeRef(const std::string &devname, const std::string &thread_tok) {
    assert(d->mythread == pthread_self());
    int refcnt = -1;
    auto rit = d->m_devmap.equal_range(devname);
    auto it = rit.first;
    while(it != rit.second) {
        if(it->second.thread_token == thread_tok) {
            TDevice *td = it->second.tdevice;
            refcnt = td->removeRef();
            if(refcnt == 0) { // no more references for that device in that thread
                delete td;
                it = d->m_devmap.erase(it);
            }
            break;
        }
        else
            ++it;
    }
    return refcnt;
}
