#include "cumbiahttpworld.h"
#include "cuhttpactionreader.h"
#include "cuhttpactionconf.h"
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
    QString url, src, prepared_src;
    bool exit;
    CuData property_d, value_d, options;
    QNetworkAccessManager *networkAccessManager;
    ProtocolHelper_I *proto_helper_i;
    CuHttpProtocolHelpers *proto_helpers;
};

CuHttpActionConf::CuHttpActionConf(const CuHTTPSrc &src, QNetworkAccessManager *qnam, const QString &url, CuHttpAuthManager *)
    : CuHTTPActionA(qnam)
{
    d = new CuHttpActionConfPrivate;
    d->networkAccessManager = qnam;
    d->url = url;
    d->src = QString::fromStdString(src.getName());
    d->prepared_src = QString::fromStdString(src.prepare());
}

CuHttpActionConf::~CuHttpActionConf() {
    pdelete("~CuHttpActionConf \"%s\" %p", qstoc(d->src), this);
    delete d;
}

QString CuHttpActionConf::getSourceName() const {
    return d->src;
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
    const QString src = QString("/x/conf/%1").arg(d->prepared_src.isEmpty() ?  d->src : d->prepared_src);
    cuprintf("\e[1;36mCuHttpActionConf::start %s\e[0m\n", qstoc(QString(d->url + src)));
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
    CuData res("src", d->src.toStdString());
    CumbiaHTTPWorld httpw;
    httpw.json_decode(json, res);
    d->exit = true;
    cuprintf("\e[1;36mCuHttpActionConf::decodeMessage %ld listeners\e[0m\n", d->listeners.size());
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        (*it)->onUpdate(res);
    }
    d->listeners.clear();
}
