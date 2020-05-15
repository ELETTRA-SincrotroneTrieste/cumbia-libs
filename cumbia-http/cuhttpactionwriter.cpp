#include "cumbiahttpworld.h"
#include "cuhttpactionreader.h"
#include "cuhttpactionwriter.h"
#include "cuhttp_source.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include "cuhttpprotocolhelpers.h"
#include "cuhttpprotocolhelper_i.h"
#include "cuhttp_source.h"
#include <set>

#include <cudatalistener.h>
#include <qustring.h>

class CuHTTPActionWriterPrivate
{
public:
    CuHTTPActionWriterPrivate() : exit(false), nam(nullptr),
        proto_helper_i(nullptr), proto_helpers(nullptr) {

    }

    std::set<CuDataListener *> listeners;
    HTTPSource http_target;
    QString url;
    bool exit;
    CuData options;
    QNetworkAccessManager *nam;
    ProtocolHelper_I *proto_helper_i;
    CuHttpProtocolHelpers *proto_helpers;
    CuVariant w_val;
};

CuHttpActionWriter::CuHttpActionWriter(const HTTPSource &target, QNetworkAccessManager *qnam, const QString &http_url)
 : CuHTTPActionA(qnam)
{
    d = new CuHTTPActionWriterPrivate;
    d->nam = qnam;
    d->url = http_url;
    d->http_target = target;
}

CuHttpActionWriter::~CuHttpActionWriter()
{
    pdelete("~CuHttpActionWriter \"%s\" %p", d->http_target.getName().c_str(), this);
    delete d;
}

void CuHttpActionWriter::setWriteValue(const CuVariant &w) {
    d->w_val = w;
}

void CuHttpActionWriter::setConfiguration(const CuData &co) {
    d->options = co;
}

void CuHttpActionWriter::onNetworkReplyFinished(QNetworkReply *) {

}

HTTPSource CuHttpActionWriter::getSource() const {
    return d->http_target;
}

CuHTTPActionA::Type CuHttpActionWriter::getType() const {
    return CuHTTPActionA::Writer;
}

void CuHttpActionWriter::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuHttpActionWriter::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuHttpActionWriter::dataListenersCount() {
    return d->listeners.size();
}

void CuHttpActionWriter::start() {
    QString src;
    QString url_s = QString::fromStdString(d->http_target.getName());
    src = QString("/x/write/%1").arg(url_s);
    if(d->w_val.isValid())
        src += QString("(%1)").arg(d->w_val.toString().c_str());
    startRequest(d->url + src);
}

bool CuHttpActionWriter::exiting() const {
    return d->exit;
}

void CuHttpActionWriter::stop() {
    d->exit = true;
    d->listeners.clear();
}

void CuHttpActionWriter::decodeMessage(const QJsonDocument &json) {
    CuData res("src", d->http_target.getName());
    CumbiaHTTPWorld httpw;
    httpw.json_decode(json, res);
    d->exit = res["is_result"].toBool();
    if(res["err"].toBool() || res["is_result"].toBool()) {
        for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
            (*it)->onUpdate(res);
            cuprintf("CuHttpActionWriter::decodeMessage: updating with result %s\n", datos(res));
        }
    }
    if(d->exit) d->listeners.clear();
}
