#ifndef CUMBIAWEBSOCKET_H
#define CUMBIAWEBSOCKET_H

#include <cumbia.h>
#include <string>
#include <cuwsactionreader.h>
#include <cuwsactionfactoryi.h>
#include "cumbia-websocket_global.h"
#include <cuwsclient.h>
#include <QString>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;

class CumbiaWebSocketPrivate;

class CUMBIAWEBSOCKETSHARED_EXPORT CumbiaWebSocket : public Cumbia, public CuWSClientListener
{

public:
    enum Type { CumbiaWSType = Cumbia::CumbiaUserType + 12 };

    CumbiaWebSocket(const QString& websocket_url,
                    const QString& http_url,
                    CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaWebSocket();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    QString httpUrl() const;

    QString websocketUrl() const;

    virtual int getType() const;

    CuWSClient *websocketClient() const;

    void addAction(const std::string &source, CuDataListener *l, const CuWSActionFactoryI &f);
    void unlinkListener(const string &source, CuWSActionI::Type t, CuDataListener *l);
    CuWSActionI *findAction(const std::string &source, CuWSActionI::Type t) const;

    // CuWSClientListener interface
public:
    virtual void onUpdate(const QString& message);

private:

    void m_init();

    CumbiaWebSocketPrivate *d;

};

#endif // CUMBIAWEBSOCKET_H
