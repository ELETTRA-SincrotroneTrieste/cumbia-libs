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
    CuData property_d, value_d, options;
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
    if(!src.canMonitor()) d->options.set("method", "read");
    d->prepared_http_src = QString::fromStdString(src.prepare());
    printf("CuHTTPActionReader: found protocol \"%s\" within \"%s\" -- options %s -- prepared src: %s\n",
           proto.c_str(), src.getName().c_str(), datos(d->options), qstoc(d->prepared_http_src));
}

CuHTTPActionReader::~CuHTTPActionReader() {
    pdelete("~CuHTTPActionReader \"%s\" %p", qstoc(d->http_src), this);
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
    return CuHTTPActionA::Reader;
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

void CuHTTPActionReader::decodeMessage(const QJsonDocument &json) {
    CuData res = getToken();
    CumbiaHTTPWorld httpw;
    httpw.json_decode(json, res);
    std::set<CuDataListener *> lcp = d->listeners;
    for(std::set<CuDataListener *>::iterator it = lcp.begin(); it != lcp.end(); ++it)
        (*it)->onUpdate(res);
}

bool CuHTTPActionReader::exiting() const {
    return d->exit;
}

void CuHTTPActionReader::mergeOptions(const CuData &o) {
    d->options.merge(o);
}

void CuHTTPActionReader::onUnsubscribeReplyFinished() {
    // stopRequest -> CuHttpActionA::d->reply->close()
    // --> SLOT(CuHttpActionA::onReplyFinished)
    // --> CumbiaHttp::onActionFinished (CumbiaHttp is a CuHTTPActionListener)
    // --> CumbiaHttp unregisters and deletes this
    sender()->deleteLater();
    stopRequest();
}

void CuHTTPActionReader::start() {
    QString url_s = !d->prepared_http_src.isEmpty() ? d->prepared_http_src : d->http_src;
    url_s.replace("#", "%23");
    QString src;
    if(d->options["method"].toString() == "read") {
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
    QString url_s = !d->prepared_http_src.isEmpty() ? d->prepared_http_src : d->http_src;
    QString src = QString("/u/%1/%2").arg(d->chan_recv->channel()).arg(url_s);
    d->chan_recv->unregisterReader(url_s);
    printf("CuHttpActionReader.stop: requesting unsubscribe: %s\n", qstoc(QString(d->url + src)));
    QNetworkRequest r = prepareRequest(d->url + src);
    QNetworkReply *reply = getNetworkAccessManager()->get(r);
    connect(reply, SIGNAL(finished()), this, SLOT(onUnsubscribeReplyFinished()));
}

