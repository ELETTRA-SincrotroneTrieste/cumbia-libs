#include "cumbiawsworld.h"
#include "cuwsactionreader.h"
#include "cuwsactionwriterconf.h"

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

class CuWsActionWriterConfPrivate
{
public:
    std::set<CuDataListener *> listeners;
    WSSource wsconf_src;
    CuWSClient *ws_client;
    QString http_url;
    bool exit;
    CuData property_d, value_d, options;
    QNetworkAccessManager *networkAccessManager;
    CuWsProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
};

CuWsActionWriterConf::CuWsActionWriterConf(const WSSource &src, CuWSClient *wscli, const QString &http_url)
{
    d = new CuWsActionWriterConfPrivate;
    d->ws_client = wscli;
    d->http_url = http_url;
    d->wsconf_src = src;
}

void CuWsActionWriterConf::onNetworkReplyFinished(QNetworkReply *) {

}

WSSource CuWsActionWriterConf::getSource() const {
    return d->wsconf_src;
}

CuWSActionI::Type CuWsActionWriterConf::getType() const {
    return CuWSActionI::WriterConfig;
}

void CuWsActionWriterConf::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuWsActionWriterConf::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuWsActionWriterConf::dataListenersCount() {
    return d->listeners.size();
}

void CuWsActionWriterConf::start() {
    QString url_s = QString::fromStdString(d->wsconf_src.getName());
    qDebug() << __PRETTY_FUNCTION__ << "write url is " << url_s << "http_url is " << d->http_url;
    if(d->http_url.isEmpty()) { // communicate over websocket only
        QString msg = QString("CONF %1").arg(url_s);
        d->ws_client->sendMessage(msg);
    }
    else {
        perr("CuWsActionWriterConf.start: write over http not implemented yet: %s", qstoc(url_s));
    }
}

bool CuWsActionWriterConf::exiting() const {
    return d->exit;
}

void CuWsActionWriterConf::stop() {
    d->exit = true;
}

void CuWsActionWriterConf::decodeMessage(const QJsonDocument &json) {
    qDebug () << __PRETTY_FUNCTION__ << "decoding " << json;
    CuData res("src", d->wsconf_src.getName());
    CumbiaWSWorld wsw;
    wsw.json_decode(json, res);
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        printf("\e[1;CuWsActionWriterConf.decodeMessage: posting update %s on listener %p\e[0m\n",
               res.toString().c_str(), (*it));
        (*it)->onUpdate(res);
    }
}
