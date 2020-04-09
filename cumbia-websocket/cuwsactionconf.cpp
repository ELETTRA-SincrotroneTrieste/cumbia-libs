#include "cumbiawsworld.h"
#include "cuwsactionreader.h"
#include "cuwsactionconf.h"

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
    CuWsActionWriterConfPrivate() : exit(false), networkAccessManager(nullptr),
        proto_helper_i(nullptr), proto_helpers(nullptr), ws_client(nullptr) {

    }
    std::set<CuDataListener *> listeners;
    WSSource wsconf_src;
    QString http_url;
    bool exit;
    CuData property_d, value_d, options;
    QNetworkAccessManager *networkAccessManager;
    ProtocolHelper_I *proto_helper_i;
    CuWsProtocolHelpers *proto_helpers;
    CuWSClient *ws_client;
};

CuWsActionConf::CuWsActionConf(const WSSource &src, CuWSClient *wscli, const QString &http_url)
{
    d = new CuWsActionWriterConfPrivate;
    d->ws_client = wscli;
    d->http_url = http_url;
    d->wsconf_src = src;
}

CuWsActionConf::~CuWsActionConf()
{
    pdelete("~CuWsActionConf %p", this);
    if(d->networkAccessManager) delete d->networkAccessManager;
    delete d;
}

void CuWsActionConf::onNetworkReplyFinished(QNetworkReply *) {

}

WSSource CuWsActionConf::getSource() const {
    return d->wsconf_src;
}

CuWSActionI::Type CuWsActionConf::getType() const {
    return CuWSActionI::WriterConfig;
}

void CuWsActionConf::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuWsActionConf::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuWsActionConf::dataListenersCount() {
    return d->listeners.size();
}

void CuWsActionConf::start() {
    QString url_s = QString::fromStdString(d->wsconf_src.getName());
    if(d->http_url.isEmpty()) { // communicate over websocket only
        QString msg = QString("CONF %1").arg(url_s);
        d->ws_client->sendMessage(msg);
    }
    else {
        perr("CuWsActionWriterConf.start: write over http not implemented yet: %s", qstoc(url_s));
    }
}

bool CuWsActionConf::exiting() const {
    return d->exit;
}

void CuWsActionConf::stop() {
    d->listeners.clear();
    d->exit = true;
}

void CuWsActionConf::decodeMessage(const QJsonDocument &json) {
    CuData res("src", d->wsconf_src.getName());
    CumbiaWSWorld wsw;
    wsw.json_decode(json, res);
    printf("\e[1;34mCuWsActionConf::decodeMessage got data %s\e[0m\n", res.toString().c_str());
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        (*it)->onUpdate(res);
    }
    stop(); // activate exit flag
}
