#include "cuhttpactionreader.h"
#include "cuhttpactionfactoryservice.h"
#include "cuhttpprotocolhelper_i.h"
#include "cuhttpprotocolhelpers.h"
#include "cumbiahttpworld.h"
#include "cuhttpactionreader.h"
#include "cuhttpchannelreceiver.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <math.h>

#include <cumacros.h>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QtDebug>
#include <QString>

// json
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

class CuHTTPActionReaderPrivate {
public:
    std::set<CuDataListener *> listeners;
    QString url, http_src, prepared_http_src;
    bool exit;
    CuData property_d, value_d, o;
    CuHttpProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
    CuHttpChannelReceiver *chan_recv;
};

CuHTTPActionReader::CuHTTPActionReader(const CuHTTPSrc& src,
                                       CuHttpChannelReceiver *chan_recv,
                                       QNetworkAccessManager *qnam,
                                       const QString& url,
                                       CuHttpAuthManager *aman)
    : CuHTTPActionA(qnam) {
    (void) aman;
    d = new CuHTTPActionReaderPrivate;
    d->http_src = QString::fromStdString(src.getName());
    d->url = url;
    d->exit = false;  // set to true by stop
    d->chan_recv = chan_recv;
    std::string proto = src.getProtocol(); // tango:// ?
    d->proto_helpers = new CuHttpProtocolHelpers();
    d->proto_helper_i = d->proto_helpers->get(QString::fromStdString(proto));
    if(!src.canMonitor()) d->o.set("method", "read");
    d->prepared_http_src = QString::fromStdString(src.prepare());
    cuprintf("\e[1;32m+++ CuHTTPActionReader %p as  CuHttpActionA %p src %s\e[0m\n", this, qobject_cast<CuHTTPActionA *>(this), src.getName().c_str());
}

CuHTTPActionReader::~CuHTTPActionReader() {
    pdelete("~CuHTTPActionReader \"%s\" %p as CuHttpActionA %p", qstoc(d->http_src), this,
            qobject_cast<CuHTTPActionA *>(this));
    d->listeners.clear();
    if(d->proto_helpers)
        delete d->proto_helpers; // deletes its ProtocolHelper_I's
    delete d;
}

/*! \brief returns the CuData storing the token that identifies this action
 *
 * @return a CuData with the following key/value pairs
 *
 * \li "source" --> the name of the source (string)
 * \li "type" --> a constant string: "reader"
 */
CuData CuHTTPActionReader::getToken() const {
    CuData da("src", d->http_src.toStdString());
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
QString CuHTTPActionReader::getSourceName() const {
    return d->http_src;
}

CuHTTPActionA::Type CuHTTPActionReader::getType() const {
    return d->o.has("method", "read") ? CuHTTPActionA::SingleShotReader : CuHTTPActionA::Reader;
}

void CuHTTPActionReader::addDataListener(CuDataListener *l) {
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuHTTPActionReader::removeDataListener(CuDataListener *l) {
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuHTTPActionReader::dataListenersCount() {
    return d->listeners.size();
}

/* This method is called from both CuHttpChannelReceiver::decodeMessage and CuHTTPActionA::m_on_buf_complete
 * if the action type is CuHTTPActionA::Reader, from CuHTTPActionA::m_on_buf_complete only if type is
 * CuHTTPActionA::SingleShotReader.
 * Registered CuDataListener instances are updated, and if the type is *single shot*, the exit flag is set to
 * true. In this latter case, CuHTTPActionA::onReplyDestroyed will lead to action disposal.
 */
void CuHTTPActionReader::decodeMessage(const QJsonValue &v) {
    CuData res = getToken();
    CumbiaHTTPWorld httpw;
    httpw.json_decode(v, res);
    std::set<CuDataListener *> lcp = d->listeners;
    for(std::set<CuDataListener *>::iterator it = lcp.begin(); it != lcp.end(); ++it)
        (*it)->onUpdate(res);
    if(getType() == CuHTTPActionA::SingleShotReader) {
        d->exit = true;
        d->listeners.clear();
    }
}

bool CuHTTPActionReader::exiting() const {
    return d->exit;
}

void CuHTTPActionReader::mergeOptions(const CuData &o) {
    d->o.merge(o);
}

void CuHTTPActionReader::onUnsubscribeReplyFinished() {
    // stopRequest -> CuHttpActionA::d->reply->close()
    // --> SLOT(CuHttpActionA::onReplyFinished)
    // --> CumbiaHttp::onActionFinished (CumbiaHttp is a CuHTTPActionListener)
    // --> CumbiaHttp unregisters and deletes this
    sender()->deleteLater();
    stopRequest();
}

void CuHTTPActionReader::onUnSubscribeReplyDestroyed(QObject *) {
    notifyActionFinished();
}

void CuHTTPActionReader::start() {
    QString url_s = !d->prepared_http_src.isEmpty() ? d->prepared_http_src : d->http_src;
    url_s.replace("#", "%23");
    QString src;
    if(d->o.has("method", "read")) {
        src = QString("/x/read/%1").arg(url_s);
    }
    else {
        src = QString("/s/%1/p/%2").arg(d->chan_recv->channel()).arg(url_s);
        d->chan_recv->registerReader(url_s, this);
    }
    startRequest(d->url + src);
}

void CuHTTPActionReader::stop() {
    d->exit = true;
    if(getType() == CuHTTPActionA::Reader) {
        // 1. stop reading from channel
        QString url_s = !d->prepared_http_src.isEmpty() ? d->prepared_http_src : d->http_src;
        QString src = QString("/u/%1/%2").arg(d->chan_recv->channel()).arg(url_s);
        d->chan_recv->unregisterReader(url_s);
        printf("CuHttpActionReader.stop: requesting unsubscribe: %s\n", qstoc(QString(d->url + src)));
        QNetworkRequest r = prepareRequest(d->url + src);
        QNetworkReply *reply = getNetworkAccessManager()->get(r);
        connect(reply, SIGNAL(finished()), this, SLOT(onUnsubscribeReplyFinished()));
        connect(reply, SIGNAL(destroyed(QObject*)), this, SLOT(onUnSubscribeReplyDestroyed(QObject *)));
    }
    else
        cuprintf("\e[1;35mCuHTTPActionReader::stop SOURCE %s WAS NOT SUBSCRIBED TO CHANNEL -- had options %s\e[0m\n",
                 qstoc(d->http_src), datos(d->o));

    // 2. if the reply from CuHTTPActionA::startRequest is still pending, cancel it
    cancelRequest();
}

