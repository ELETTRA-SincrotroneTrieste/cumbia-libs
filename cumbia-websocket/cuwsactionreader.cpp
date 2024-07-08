#include "cuwsactionreader.h"
#include "cuwsclient.h"
#include "cuwsactionfactoryservice.h"
#include "cuwsprotocolhelper_i.h"
#include "cuwsprotocolhelpers.h"
#include "cumbiawsworld.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <math.h>

#include "cuwsactionreader.h"
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

WSSourceConfiguration::WSSourceConfiguration()
{
    m_keys   << "min_value" << "max_value" << "dt" << "display_unit" << "format" << "df";
}

void WSSourceConfiguration::add(const QString &key, const QString &value)
{
    m_map.insert(key, value);
}

bool WSSourceConfiguration::isComplete() const
{
    foreach(QString key, m_keys) {
        if(!m_map.contains(key))
            return false;
    }
    return true;
}

CuData WSSourceConfiguration::toCuData() const
{
    CuData res;
    res[TTT::Min] = m_map["min_value"].toStdString();  // res["min"]
    res[TTT::Max] = m_map["max_value"].toStdString();  // res["max"]
    res["display_unit"] = m_map["display_unit"].toStdString();
    res[TTT::NumberFormat] =
        m_map["format"].toStdString();  // !cudata
    res[TTT::DataType] =
        m_map["dt"].toInt();  // !cudata
    res[TTT::DataFormat] =
        m_map["df"].toInt();  //  !cudata
    return res;
}

QStringList WSSourceConfiguration::keys() const {
    return m_keys;
}

void WSSourceConfiguration::setError(const QString &message)
{
    m_errorMsg = message;
}

bool WSSourceConfiguration::error() const
{
    return !m_errorMsg.isEmpty();
}

QString WSSourceConfiguration::errorMessage() const
{
    return m_errorMsg;
}


class CuWSActionReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    WSSource tsrc;
    CuWSClient *ws_client;
    QString http_url;
    bool exit;
    CuData property_d, value_d, options;
    QNetworkAccessManager *networkAccessManager;
    WSSourceConfiguration source_configuration;
    CuWsProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
};

CuWSActionReader::CuWSActionReader(const WSSource& src, CuWSClient *wscli, const QString& http_url) : CuWSActionI()
{
    d = new CuWSActionReaderPrivate;
    d->tsrc = src;
    d->ws_client = wscli;
    d->http_url = http_url;
    d->exit = false;  // set to true by stop
    d->networkAccessManager = nullptr;
    std::string proto = src.getProtocol(); // tango:// ?
    pinfo("CuWSActionReader: found protocol \"%s\" within \"%s\"", proto.c_str(), src.getName().c_str());
    d->proto_helpers = new CuWsProtocolHelpers();
    d->proto_helper_i = d->proto_helpers->get(QString::fromStdString(proto));
}

CuWSActionReader::~CuWSActionReader()
{
    pdelete("~CuWSActionReader \"%s\" %p", d->tsrc.getName().c_str(), this);
    if(d->networkAccessManager)
        delete d->networkAccessManager;
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
CuData CuWSActionReader::getToken() const
{
    CuData da(TTT::Src, d->tsrc.getName());  // CuData da("src", d->tsrc.getName()
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
WSSource CuWSActionReader::getSource() const
{
    return d->tsrc;
}

CuWSActionI::Type CuWSActionReader::getType() const
{
    return CuWSActionI::Reader;
}

void CuWSActionReader::addDataListener(CuDataListener *l)
{
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuWSActionReader::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuWSActionReader::dataListenersCount()
{
    return d->listeners.size();
}

void CuWSActionReader::decodeMessage(const QJsonDocument &json)
{
    CuData res = getToken();
    CumbiaWSWorld wsw;
    wsw.json_decode(json, res);
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        (*it)->onUpdate(res);
    }
}


bool CuWSActionReader::exiting() const {
    return d->exit;
}

void CuWSActionReader::setOptions(const CuData &o) {
    d->options = o;
}

void CuWSActionReader::onNetworkReplyFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QString requrl = reply->request().url().toString();
    if(requrl == d->http_url + QString::fromStdString(d->tsrc.getName())) {
        // we got the first reading of the source
        QJsonParseError jpe;
        QJsonDocument jd = QJsonDocument::fromJson(data, &jpe);
        if(!jd.isNull())
            decodeMessage(jd);
    }
    else {
        if(reply->error() != QNetworkReply::NoError) {
            d->source_configuration.setError(reply->errorString());
            perr("CuWSActionReader.onNetworkReplyFinished: %s", qstoc(reply->errorString()));
        }
        else {
            QString key = requrl.section('/', -1);
            d->source_configuration.add(key, QString(data).remove("\""));
            if(d->source_configuration.isComplete()) {
                CuData conf = d->source_configuration.toCuData();
                conf[TTT::Src] = d->tsrc.getName();  // conf["src"]
                conf[TTT::Err] = false; //d->source_configuration.error();  // conf["err"]
                if(d->proto_helper_i)
                    conf[TTT::DataFormatStr] = d->proto_helper_i->dataFormatToStr(conf[TTT::DataFormat].toInt());  // conf["dfs"], conf["df"]
                conf[TTT::Message] = d->source_configuration.errorMessage().toStdString();  // conf["msg"]
                conf[TTT::Type] = "property";  // conf["type"]

                for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
                    (*it)->onUpdate(conf);
                }
            }
        }
    }
}

void CuWSActionReader::start()
{
    QString url_s = QString::fromStdString(d->tsrc.getName());
    if(d->http_url.isEmpty()) {
        // communicate over websocket only
        QString msg = QString("SUBSCRIBE %1").arg(url_s);
        d->ws_client->sendMessage(msg);
        msg = QString("RCONF %1").arg(url_s);
        d->ws_client->sendMessage(msg);
    }
    else {
        d->networkAccessManager = new QNetworkAccessManager(this);
        connect(d->networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyFinished(QNetworkReply*)));

        // get configuration parameters for the source (pwma/canoned)
        // This implies fetching each property with a dedicated GET
        if(!d->options["auto_configuration"].toBool()) {
            WSSourceConfiguration soco;
            QStringList keys = soco.keys();
            QNetworkRequest config_r;
            foreach(QString key, keys) {
                config_r.setUrl(QUrl(d->http_url + url_s + "/" + key));
                d->networkAccessManager->get(config_r);
            }
        }

        // 2. subscribe to events

        QNetworkRequest subscribe_request(d->http_url + url_s);
        //    subscribe_request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        d->networkAccessManager->sendCustomRequest(subscribe_request, QByteArray("SUBSCRIBE"));
    }
}

void CuWSActionReader::stop()
{
    if(!d->exit)
        d->exit = true;

    CuData tok = getToken();
    QString url_s = QString::fromStdString(tok[TTT::Src].toString());  // tok["src"]
    // unsubscribe
    if(d->networkAccessManager) { // created in start if d->http_url is not empty
        QNetworkRequest unsubscribe_request;
        unsubscribe_request.setUrl(QUrl(d->http_url + url_s));
        //        unsubscribe_request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        pviolet2tmp("CuWSActionReader::stop: unsubscribing from \"%s\"", qstoc(unsubscribe_request.url().toString()));
        d->networkAccessManager->sendCustomRequest(unsubscribe_request, QByteArray("UNSUBSCRIBE"));

        delete d->networkAccessManager;
        d->networkAccessManager = NULL;
    }
    else {
        // communicate over websocket only
        QString msg = QString("UNSUBSCRIBE %1").arg(url_s);
        d->ws_client->sendMessage(msg);
    }
    this->deleteLater();
}

