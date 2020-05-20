#include "cumbiahttpworld.h"
#include "cuhttpactionreader.h"
#include "cuhtttpactionconf.h"
#include "cuhttp_source.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QObject>
#include "cuhttpprotocolhelpers.h"
#include "cuhttpprotocolhelper_i.h"
#include "cuhttp_source.h"
#include <set>

#include <cudatalistener.h>
#include <qustring.h>

class CuHttpActionConfPrivate
{
public:
    CuHttpActionConfPrivate() : exit(false), networkAccessManager(nullptr),
        proto_helper_i(nullptr), proto_helpers(nullptr) {

    }
    std::set<CuDataListener *> listeners;
    HTTPSource httpconf_src;
    QString url;
    bool exit;
    CuData property_d, value_d, options;
    QNetworkAccessManager *networkAccessManager;
    ProtocolHelper_I *proto_helper_i;
    CuHttpProtocolHelpers *proto_helpers;
};

CuHttpActionConf::CuHttpActionConf(const HTTPSource &src, QNetworkAccessManager *qnam, const QString &url)
    : CuHTTPActionA(qnam)
{
    d = new CuHttpActionConfPrivate;
    d->networkAccessManager = qnam;
    d->url = url;
    d->httpconf_src = src;
}

CuHttpActionConf::~CuHttpActionConf() {
    pdelete("~CuHttpActionConf \"%s\" %p", d->httpconf_src.getName().c_str(), this);
    delete d;
}

HTTPSource CuHttpActionConf::getSource() const {
    return d->httpconf_src;
}

CuHTTPActionA::Type CuHttpActionConf::getType() const {
    return CuHTTPActionA::Config;
}

void CuHttpActionConf::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuHttpActionConf::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuHttpActionConf::dataListenersCount() {
    return d->listeners.size();
}

void CuHttpActionConf::start() {
    QString src;
    QString url_s = QString::fromStdString(d->httpconf_src.getName());
    src = QString("/x/conf/%1").arg(url_s);
    startRequest(d->url + src);
}

bool CuHttpActionConf::exiting() const {
    cuprintf("CuHttpActionConf.exiting: d->exit is %d\n", d->exit);
    return d->exit;
}

void CuHttpActionConf::stop() {
    d->listeners.clear();
    cuprintf("CuHttpActionConf.stop: setting d->exit to true\n");
    d->exit = true;
}

void CuHttpActionConf::decodeMessage(const QJsonDocument &json) {
    CuData res("src", d->httpconf_src.getName());
    CumbiaHTTPWorld httpw;
    httpw.json_decode(json, res);
    d->exit = true;
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        (*it)->onUpdate(res);
    }
    d->listeners.clear();
}
