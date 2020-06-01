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
    HTTPSource tsrc;
    QString url;
    bool exit;
    CuData property_d, value_d, options;
    CuHttpProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
    CuHttpChannelReceiver *chan_recv;
};

CuHTTPActionReader::CuHTTPActionReader(const HTTPSource& src, CuHttpChannelReceiver *chan_recv, QNetworkAccessManager *qnam, const QString& url)
    : CuHTTPActionA(qnam) {
    d = new CuHTTPActionReaderPrivate;
    d->tsrc = src;
    d->url = url;
    d->exit = false;  // set to true by stop
    d->chan_recv = chan_recv;
    std::string proto = src.getProtocol(); // tango:// ?
    pinfo("CuHTTPActionReader: found protocol \"%s\" within \"%s\"", proto.c_str(), src.getName().c_str());
    d->proto_helpers = new CuHttpProtocolHelpers();
    d->proto_helper_i = d->proto_helpers->get(QString::fromStdString(proto));
}

CuHTTPActionReader::~CuHTTPActionReader() {
    pdelete("~CuHTTPActionReader \"%s\" %p", d->tsrc.getName().c_str(), this);
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
    CuData da("src", d->tsrc.getName());
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
HTTPSource CuHTTPActionReader::getSource() const {
    return d->tsrc;
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

void CuHTTPActionReader::setOptions(const CuData &o) {
    d->options = o;
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
    QString url_s = QString::fromStdString(d->tsrc.getName());
    QString src = QString("/s/%1/p/%2").arg(d->chan_recv->channel()).arg(url_s);
    startRequest(d->url + src);
    d->chan_recv->registerReader(url_s, this);
}

void CuHTTPActionReader::stop() {
    d->exit = true;
    QString url_s = QString::fromStdString(d->tsrc.getName());
    QString src = QString("/u/%1/%2").arg(d->chan_recv->channel()).arg(url_s);
    d->chan_recv->unregisterReader(url_s);
    printf("CuHttpActionReader.stop: requesting unsubscribe: %s\n", qstoc(QString(d->url + src)));
    QNetworkRequest r = prepareRequest(d->url + src);
    QNetworkReply *reply = getNetworkAccessManager()->get(r);
    connect(reply, SIGNAL(finished()), this, SLOT(onUnsubscribeReplyFinished()));
}
