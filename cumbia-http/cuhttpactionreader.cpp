#include "cuhttpactionreader.h"
#include "cuhttpactionfactoryservice.h"
#include "cuhttpprotocolhelper_i.h"
#include "cuhttpprotocolhelpers.h"
#include "cumbiahttpworld.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <math.h>

#include "cuhttpactionreader.h"
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

HTTPSourceConfiguration::HTTPSourceConfiguration()
{
    m_keys   << "min_value" << "max_value" << "data_type" << "display_unit" << "format" << "data_format";
}

void HTTPSourceConfiguration::add(const QString &key, const QString &value)
{
    m_map.insert(key, value);
}

bool HTTPSourceConfiguration::isComplete() const
{
    foreach(QString key, m_keys) {
        if(!m_map.contains(key))
            return false;
    }
    return true;
}

CuData HTTPSourceConfiguration::toCuData() const
{
    CuData res;
    res["min"] = m_map["min_value"].toStdString();
    res["max"] = m_map["max_value"].toStdString();
    res["display_unit"] = m_map["display_unit"].toStdString();
    res["format"] = m_map["format"].toStdString();
    res["data_type"] = m_map["data_type"].toInt();
    res["data_format"] = m_map["data_format"].toInt();
    return res;
}

QStringList HTTPSourceConfiguration::keys() const {
    return m_keys;
}

void HTTPSourceConfiguration::setError(const QString &message)
{
    m_errorMsg = message;
}

bool HTTPSourceConfiguration::error() const
{
    return !m_errorMsg.isEmpty();
}

QString HTTPSourceConfiguration::errorMessage() const
{
    return m_errorMsg;
}


class CuHTTPActionReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    HTTPSource tsrc;
    QString url;
    bool exit;
    CuData property_d, value_d, options;
    HTTPSourceConfiguration source_configuration;
    CuHttpProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
};

CuHTTPActionReader::CuHTTPActionReader(const HTTPSource& src, QNetworkAccessManager *qnam, const QString& url)
    : CuHTTPActionA(qnam) {
    d = new CuHTTPActionReaderPrivate;
    d->tsrc = src;
    d->url = url;
    d->exit = false;  // set to true by stop
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
HTTPSource CuHTTPActionReader::getSource() const
{
    return d->tsrc;
}

CuHTTPActionA::Type CuHTTPActionReader::getType() const
{
    return CuHTTPActionA::Reader;
}

void CuHTTPActionReader::addDataListener(CuDataListener *l)
{
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuHTTPActionReader::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuHTTPActionReader::dataListenersCount()
{
    return d->listeners.size();
}

void CuHTTPActionReader::decodeMessage(const QJsonDocument &json)
{
    CuData res = getToken();
    CumbiaHTTPWorld httpw;
    httpw.json_decode(json, res);
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        (*it)->onUpdate(res);
    }
}

bool CuHTTPActionReader::exiting() const {
    return d->exit;
}

void CuHTTPActionReader::setOptions(const CuData &o) {
    d->options = o;
}

QNetworkRequest CuHTTPActionReader::prepareRequest(const QUrl &url) const
{
    /*
     * -- sniffed from JS EventSource -- tcpdump -vvvs 1024 -l -A -i lo port 8001 -n
     * .^...^..GET /sub/subscribe/hokuto:20000/test/device/1/double_scalar HTTP/1.1
        Host: woody.elettra.eu:8001
        User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:76.0) Gecko/20100101 Firefox/76.0
        Accept: text/event-stream
        Accept-Language: en-US,en;q=0.5
        Accept-Encoding: gzip, deflate
        Origin: http://woody:8001
        DNT: 1
        Connection: keep-alive
        Referer: http://woody:8001/
        Pragma: no-cache
        Cache-Control: no-cache
    */
    QNetworkRequest r (url);
    r = CuHTTPActionA::prepareRequest(url);
    r.setRawHeader("Accept", "text/event-stream");
    r.setRawHeader("Accept-Encoding", "gzip, deflate");
    r.setRawHeader("Accept-Encoding", "gzip, deflate");
    r.setRawHeader("Connection", "keep-alive");
    r.setRawHeader("Pragma", "no-cache");
    r.setRawHeader("Cache-Control", "no-cache");

    r.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    r.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork); // Events shouldn't be cached
    return r;
}

void CuHTTPActionReader::start() {
    QString url_s = QString::fromStdString(d->tsrc.getName());
    QString src = QString("/sub/subscribe/%1").arg(url_s);
    startRequest(d->url + src);
}

void CuHTTPActionReader::stop()
{
    if(!d->exit)
        d->exit = true;
    stopRequest();

    cuprintf("CuHTTPActionReader.stop: unsubscribe request sent... what do we do now?????\n");
    //  getListener()->onActionFinished(d->tsrc.getName(), getType());
}
