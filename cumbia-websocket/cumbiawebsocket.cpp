#include "cumbiawebsocket.h"

#include <cumacros.h>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumbiawsworld.h>
#include <cuwsactionfactoryservice.h>
#include <cuwsactionfactoryi.h>
#include <QtDebug>
#include <QtWebSockets/QWebSocket>

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include "cuwsclient.h"

class CumbiaWebSocketPrivate {
public:
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
    CuWSClient *cu_wscli;
    QString http_url, ws_url;
};

CumbiaWebSocket::CumbiaWebSocket(const QString &websocket_url,
                                 const QString &http_url,
                                 CuThreadFactoryImplI *tfi,
                                 CuThreadsEventBridgeFactory_I *teb)
{
    qDebug() << "CumbiaWebSocket constructor...\n";
    d = new CumbiaWebSocketPrivate;
    d->m_threadsEventBridgeFactory = teb;
    d->m_threadFactoryImplI = tfi;
    d->http_url = http_url;
    d->ws_url = websocket_url;

    // CuWSClient waits for messages on the websocket and invokes onUpdate on this
    // upon new data
    d->cu_wscli = new CuWSClient(QUrl(websocket_url), this, NULL);

    m_init();


    // PER ORA!
    printf("\e[1;31m PER ORA APRO QUI SOCKET!!!\e[0m\n");
    d->cu_wscli->open();
}

CumbiaWebSocket::~CumbiaWebSocket()
{
    printf("~CumbiaWebSocket %p\n", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(d->m_threadsEventBridgeFactory)
        delete d->m_threadsEventBridgeFactory;
    if(d->m_threadFactoryImplI)
        delete d->m_threadFactoryImplI;
    if(d->cu_wscli)
        delete d->cu_wscli;
    delete d;
}

void CumbiaWebSocket::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuWSActionFactoryService::CuWSActionFactoryServiceType),
                                          new CuWSActionFactoryService());
    // make sure urls end with '/'
    if(!d->http_url.endsWith('/'))
        d->http_url += '/';
    if(!d->ws_url.endsWith('/'))
        d->ws_url += "/";
}

void CumbiaWebSocket::addAction(const std::string &source, CuDataListener *l, const CuWSActionFactoryI &f)
{
    CumbiaWSWorld w;
    if(w.source_valid(source))
    {
        CuWSActionFactoryService *af =
                static_cast<CuWSActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuWSActionFactoryService::CuWSActionFactoryServiceType)));

        CuWSActionI *a = af->findActive(source, f.getType());
        if(!a) {
            a = af->registerAction(source, f, this);
            a->start();
        }
        else {
            cuprintf("CumbiaWebSocket.addAction: action %p already found for source \"%s\" and type %d thread 0x%lx TYPE %d\n",
                     a, source.c_str(), f.getType(), pthread_self(), f.getType());
        }
        a->addDataListener(l);
    }
    else
        perr("CumbiaWebSocket.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

void CumbiaWebSocket::unlinkListener(const string &source, CuWSActionI::Type t, CuDataListener *l)
{
    CuWSActionFactoryService *af =
            static_cast<CuWSActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuWSActionFactoryService::CuWSActionFactoryServiceType)));
    std::vector<CuWSActionI *> actions = af->find(source, t);
    for(size_t i = 0; i < actions.size(); i++) {
        actions[i]->removeDataListener(l); /* when no more listeners, a stops itself */
    }
}

CuWSActionI *CumbiaWebSocket::findAction(const std::string &source, CuWSActionI::Type t) const
{
    CuWSActionFactoryService *af =
            static_cast<CuWSActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuWSActionFactoryService::CuWSActionFactoryServiceType)));
    CuWSActionI* a = af->findActive(source, t);
    return a;
}

CuThreadFactoryImplI *CumbiaWebSocket::getThreadFactoryImpl() const
{
    return d->m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaWebSocket::getThreadEventsBridgeFactory() const
{
    return d->m_threadsEventBridgeFactory;
}

QString CumbiaWebSocket::httpUrl() const {
     return d->http_url;
}

QString CumbiaWebSocket::websocketUrl() const {
    return d->ws_url;
}

int CumbiaWebSocket::getType() const {
    return CumbiaWSType;
}

CuWSClient *CumbiaWebSocket::websocketClient() const
{
    return d->cu_wscli;
}

/*! \brief Callback invoked by CuWSClient when a new message is received from the websocket
 *
 * The received message is used to build a JSon document in order to extract the source.
 * The decoding of the message is taken over by the CuWSActionI with the given source
 *
 * @see CuWSClient::onMessageReceived
 */
void CumbiaWebSocket::onUpdate(const QString &message)
{
    // 1. extract src
    QJsonParseError jpe;
    QJsonDocument jsd = QJsonDocument::fromJson(message.toUtf8(), &jpe);
    std::string src = jsd["event"].toString().toStdString();

    // 2. find action: data from websocket is always related to readers
    CuWSActionI *action = findAction(src, CuWSActionI::Reader);
    if(action) {
        // 3. let the action decode the content (according to data format, type, and so on) and notify the listeners
        action->decodeMessage(jsd);
    }
}
