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
            refcnt = it->second->tdevice->removeRef();
            if(refcnt == 0) {
                printf("\e[1;31mXXX \e[0m~CuTThreadDevicesPrivate: deleting dev for %s\n", it->first.c_str());
                delete it->second->tdevice;
            }
            else {
                 printf("\e[0;35mXXX  still %d references to %s\n", it->first.c_str());
            }
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
    printf("CuTThreadDevices: \e[1;32mdevice %s ref %d\e[0m\n", name.c_str(), td->refCnt());
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
            printf("CuTThreadDevices: \e[1;35mdevice %s ref %d\e[0m --> ", devname.c_str(), td->refCnt());
            if(refcnt == 0) { // no more references for that device in that thread
                printf(" \e[1;35m deleting\e[0m\n");
                delete td;
                it = d->m_devmap.erase(it);
            }
            else {
                printf(" \e[0;33m NOT deleting\e[0m\n");

            }
            break;
        }
        else
            ++it;
    }
    return refcnt;
}
