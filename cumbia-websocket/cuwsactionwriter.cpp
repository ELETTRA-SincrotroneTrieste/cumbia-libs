#include "cumbiawsworld.h"
#include "cuwsactionreader.h"
#include "cuwsactionwriter.h"
#include "cuwsclient.h"
#include "ws_source.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include "cuwsprotocolhelpers.h"
#include "cuwsprotocolhelper_i.h"
#include "ws_source.h"
#include <set>

#include <cudatalistener.h>
#include <qustring.h>

class CuWSActionWriterPrivate
{
public:
    std::set<CuDataListener *> listeners;
    WSSource ws_target;
    CuWSClient *ws_client;
    QString http_url;
    bool exit;
    CuData property_d, value_d, options;
    QNetworkAccessManager *networkAccessManager;
    CuWsProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
    CuVariant w_val;
};

CuWsActionWriter::CuWsActionWriter(const WSSource &target, CuWSClient *wscli, const QString &http_url)
{
    d = new CuWSActionWriterPrivate;
    d->ws_client = wscli;
    d->http_url = http_url;
    d->ws_target = target;
}

void CuWsActionWriter::setWriteValue(const CuVariant &w) {
    d->w_val = w;
}

void CuWsActionWriter::setConfiguration(const CuData &co) {
    d->options = co;
}

void CuWsActionWriter::onNetworkReplyFinished(QNetworkReply *) {

}

WSSource CuWsActionWriter::getSource() const {
    return d->ws_target;
}

CuWSActionI::Type CuWsActionWriter::getType() const {
    return CuWSActionI::Writer;
}

void CuWsActionWriter::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuWsActionWriter::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuWsActionWriter::dataListenersCount() {
    return d->listeners.size();
}

void CuWsActionWriter::start() {
    QString url_s = QString::fromStdString(d->ws_target.getName());
    url_s += "=";
    if(!d->w_val.isNull())
        url_s += QuString(d->w_val.toString());
    qDebug() << __PRETTY_FUNCTION__ << "write url is " << url_s << "http_url is " << d->http_url;
    if(d->http_url.isEmpty()) {
        // communicate over websocket only
        QString msg = QString("%1").arg(url_s);
        d->ws_client->sendMessage(msg);
    }
    else {
        perr("CuWsActionWriter.start: write over http not implemented yet: %s", qstoc(url_s));
    }
}

bool CuWsActionWriter::exiting() const {
    return d->exit;
}

void CuWsActionWriter::stop() {
    d->exit = true;
}

void CuWsActionWriter::decodeMessage(const QJsonDocument &json) {
    qDebug () << __PRETTY_FUNCTION__ << "decoding " << json;
    CuData res("src", d->ws_target.getName());
    CumbiaWSWorld wsw;
    wsw.json_decode(json, res);
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        printf("\e[1;CuWsActionWriter.decodeMessage: posting update %s on listener %p\e[0m\n",
               res.toString().c_str(), (*it));
        (*it)->onUpdate(res);
    }
}
