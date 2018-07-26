#ifndef CUMBIAWEBSOCKET_H
#define CUMBIAWEBSOCKET_H

#include <cumbia.h>
#include <string>
#include <cuwsactionreader.h>
#include <cuwsactionfactoryi.h>
#include "cumbia-websocket_global.h"

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;

class CUMBIAWEBSOCKETSHARED_EXPORT CumbiaWebSocket : public Cumbia
{

public:
    enum Type { CumbiaWSType = Cumbia::CumbiaUserType + 12 };

    CumbiaWebSocket(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaWebSocket();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

    void addAction(const std::string &source, CuDataListener *l, const CuWSActionFactoryI &f);
    void unlinkListener(const string &source, CuWSActionI::Type t, CuDataListener *l);
    CuWSActionI *findAction(const std::string &source, CuWSActionI::Type t) const;
private:

    void m_init();
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIAWEBSOCKET_H
