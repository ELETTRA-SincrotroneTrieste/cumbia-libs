#include "cumbiahttp.h"

#include <cumacros.h>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumbiahttpworld.h>
#include <cuhttpactionfactoryservice.h>
#include <cuhttpactionfactoryi.h>
#include <QtDebug>
#include <QUrl>

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>
#include <qnetworkaccessmanager.h>
#include <qureplacewildcards_i.h>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QThread> // for QThread::currentThread()

class CumbiaHttpPrivate {
public:
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
    QString url;
    QList<QuReplaceWildcards_I *> m_repl_wildcards_i;
    QNetworkAccessManager *qnam;
};

/*!
 * \brief CumbiaHttp
 * \param http_url the url to be used for http incoming data
 * \param http_url the http (https) url to be used to send requests to the server (output)
 * \param tfi thread factory implementation
 * \param teb thread events bridge factory
 */
CumbiaHttp::CumbiaHttp(const QString &url,
                                 CuThreadFactoryImplI *tfi,
                                 CuThreadsEventBridgeFactory_I *teb)
{
    d = new CumbiaHttpPrivate;
    d->m_threadsEventBridgeFactory = teb;
    d->m_threadFactoryImplI = tfi;
    d->url = url;
    d->qnam = new QNetworkAccessManager(nullptr);
    cuprintf("CumbiaHttp: instantiated with url %s\n", qstoc(url));
    m_init();
}

CumbiaHttp::~CumbiaHttp()
{
    pdelete("~CumbiaHttp %p", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(d->m_threadsEventBridgeFactory)
        delete d->m_threadsEventBridgeFactory;
    if(d->m_threadFactoryImplI)
        delete d->m_threadFactoryImplI;
    delete d->qnam;
    foreach(QuReplaceWildcards_I *i, d->m_repl_wildcards_i)
        delete i;
    delete d;
}

void CumbiaHttp::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuHTTPActionFactoryService::CuHTTPActionFactoryServiceType),
                                          new CuHTTPActionFactoryService());
}

void CumbiaHttp::onActionStarted(const string &source, CuHTTPActionA::Type t) {
    Q_UNUSED(source)
    Q_UNUSED(t)
}

void CumbiaHttp::onActionFinished(const string &source, CuHTTPActionA::Type t) {
    CuHTTPActionFactoryService *af =
            static_cast<CuHTTPActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHTTPActionFactoryService::CuHTTPActionFactoryServiceType)));

    CuHTTPActionA *a = af->unregisterAction(source, t);
    qDebug() << __PRETTY_FUNCTION__ << source.c_str() << "type " << t << "found " << a;
    if(a) delete a;
}

void CumbiaHttp::addAction(const std::string &source, CuDataListener *l, const CuHTTPActionFactoryI &f)
{
    CumbiaHTTPWorld w;
    if(w.source_valid(source))
    {
        CuHTTPActionFactoryService *af =
                static_cast<CuHTTPActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHTTPActionFactoryService::CuHTTPActionFactoryServiceType)));
        CuHTTPActionA *a = af->findActive(source, f.getType());
        if(!a) {
            a = af->registerAction(source, f, d->qnam, d->url);
            qDebug() << __PRETTY_FUNCTION__ << "registered action with source " << source.c_str() << f.getType();
            a->setHttpActionListener(this);
            a->start();
        }
        else {
            cuprintf("CumbiaHttp.addAction: action %p already found for source \"%s\" and type %d thread 0x%lx TYPE %d\n",
                     a, source.c_str(), f.getType(), pthread_self(), f.getType());
        }
        a->addDataListener(l);
    }
    else
        perr("CumbiaHttp.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

void CumbiaHttp::unlinkListener(const string &source, CuHTTPActionA::Type t, CuDataListener *l)
{
    cuprintf("CumbiaHttp::unlinkListener %s %d\n", source.c_str(), t);
    CuHTTPActionFactoryService *af =
            static_cast<CuHTTPActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHTTPActionFactoryService::CuHTTPActionFactoryServiceType)));
    std::vector<CuHTTPActionA *> actions = af->find(source, t);
    for(size_t i = 0; i < actions.size(); i++) {
        actions[i]->removeDataListener(l); /* when no more listeners, a stops itself */
    }
}

CuHTTPActionA *CumbiaHttp::findAction(const std::string &source, CuHTTPActionA::Type t) const
{
    CuHTTPActionFactoryService *af =
            static_cast<CuHTTPActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHTTPActionFactoryService::CuHTTPActionFactoryServiceType)));
    CuHTTPActionA* a = af->findActive(source, t);
    return a;
}

void CumbiaHttp::addReplaceWildcardI(QuReplaceWildcards_I *rwi) {
    d->m_repl_wildcards_i << rwi;
}

QList<QuReplaceWildcards_I *> CumbiaHttp::getReplaceWildcard_Ifaces() const{
    return d->m_repl_wildcards_i;
}

CuThreadFactoryImplI *CumbiaHttp::getThreadFactoryImpl() const
{
    return d->m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaHttp::getThreadEventsBridgeFactory() const
{
    return d->m_threadsEventBridgeFactory;
}

QString CumbiaHttp::url() const {
     return d->url;
}

int CumbiaHttp::getType() const {
    return CumbiaHTTPType;
}
