#include "cuwsactionreader.h"
#include "cuwsclient.h"
#include "cuwsactionfactoryservice.h"
#include "cuwsprotocolhelper_i.h"
#include "cuwsprotocolhelpers.h"

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
    m_keys   << "min_value" << "max_value" << "data_type" << "display_unit" << "format" << "data_format";
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
    res["min"] = m_map["min_value"].toStdString();
    res["max"] = m_map["max_value"].toStdString();
    res["display_unit"] = m_map["display_unit"].toStdString();
    res["format"] = m_map["format"].toStdString();
    res["data_type"] = m_map["data_type"].toInt();
    res["data_format"] = m_map["data_format"].toInt();
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
    pdelete("~CuWSActionReader %p", this);
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
    CuData da("src", d->tsrc.getName());
    da["type"] = std::string("reader");
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
    if(l->invalid())
    {
        d->listeners.erase(l);
        if(!d->listeners.size()) {
            stop();
        }
    }
    else if(d->listeners.size() == 1) {
        stop();
    }
    else
        d->listeners.erase(l);
}

size_t CuWSActionReader::dataListenersCount()
{
    return d->listeners.size();
}

void CuWSActionReader::decodeMessage(const QJsonDocument &json)
{
    qDebug () << __PRETTY_FUNCTION__ << "decoding " << json;
    CuData res = getToken();
    if(json.isNull()) {
        res["err"] = true;
        res["msg"] = "CuWSActionReader.decodeMessage: invalid json document";
    }
    else {
        QJsonObject data_o = json["data"].toObject();
        if(!data_o.isEmpty()) {
            foreach(const QString &vk,  QStringList() << "value" << "w_value") {
                if(data_o.contains(vk)) {
                    const std::string& svk = vk.toStdString();
                    QJsonValue value = data_o[vk];
                    if(!value.isNull() && value.isArray()) {
                        res["data_format_str"] = std::string("vector");
                        QJsonArray jarr = value.toArray();
                        // decide type
                        if(jarr.size() > 0 && jarr.at(0).isDouble()) {
                            std::vector<double> vd;
                            for(int i = 0; i < jarr.size(); i++) {
                                QJsonValue ithval = jarr.at(i);
                                vd.push_back(ithval.toDouble());
                            }
                            res[svk] = vd;
                        }
                        else if(jarr.size() > 0 && jarr.at(0).isBool()) {
                            std::vector<bool> vb;
                            for(int i = 0; i < jarr.size(); i++) {
                                QJsonValue ithval = jarr.at(i);
                                vb.push_back(ithval.toBool());
                            }
                            res[svk] = vb;
                        }
                        else if(jarr.size() > 0 && jarr.at(0).isString()) {
                            std::vector<std::string> vs;
                            for(int i = 0; i < jarr.size(); i++) {
                                QJsonValue ithval = jarr.at(i);
                                vs.push_back(ithval.toString().toStdString());
                            }
                            res[svk] = vs;
                        }
                    }
                    else if(!value.isNull()){ // scalar
                        res["data_format_str"] = std::string("scalar");
                        if(value.isBool())
                            res[svk] = value.toBool();
                        else if(value.isDouble())
                            res[svk] = value.toDouble();
                        else if(value.isString())
                            res[svk] = value.toString().toStdString();
                    }
                }
            } // foreach(QString &vk..

            // timestamp
            char *endptr;
            double ts_us = -1.0f;
            if(data_o.contains("timestamp"))
                ts_us = strtod(data_o["timestamp"].toString().toStdString().c_str(), &endptr);
            else if(data_o.contains("timestamp_us"))
                ts_us = data_o["timestamp_us"].toDouble();
            if(ts_us >= 0)
                res["timestamp_us"] = ts_us;

            // timestamp millis
            if(data_o.contains("timestamp_ms")) // timestamp_ms converted to long int
                res["timestamp_ms"] = static_cast<long int>(data_o["timestamp_ms"].toDouble());
            else if(ts_us >= 0)
                res["timestamp_ms"] = static_cast<long long int>(floor(ts_us) * 1000 + (ts_us - floor(ts_us)) * 10e6 / 1000);

            if(data_o.contains("error")) {
                res["msg"] = data_o["error"].toString().toStdString();
                res["err"] = data_o["error"].toString().size() > 0;
            }
            else {
                res["err"] = data_o["err"].toBool();
                res["msg"] = data_o["msg"].toString().toStdString();
            }

            // type: property: configuration
            if(data_o["type"].toString() == "property") {
                QStringList keys = QStringList () << "abs_change" << "archive_abs_change" << "archive_period"
                                                  << "archive_rel_change" <<  "description" <<  "disp_level" << "display_unit"
                                                  << "format" << "label"<< "max" << "max_alarm"<< "max_dim_x"  << "max_dim_y" << "max_warning" << "min"
                                                  << "min_alarm" << "min_warning" << "periodic_period"  << "rel_change" << "root_attr_name"
                                                  << "standard_unit" << "writable_attr_name" << "delta_t";
                foreach(QString k, keys) {
                    if(data_o.contains(k))
                        res[k.toStdString()] = data_o[k].toString().toStdString();
                }
            }
            // quality, state
            QStringList keys = QStringList() << "data_format_str" << "state_color" << "quality_color" << "activity" << "quality_string" << "type";
            foreach(const QString& k, keys) {
                if(data_o.contains(k))
                    res[k.toStdString()] = data_o[k].toString().toStdString();
            }
            // to int
            keys = QStringList() << "state" << "quality";
            foreach(const QString& k, keys)
                if(data_o.contains(k))
                    res[k.toStdString()] = data_o[k].toInt();
        }
    }
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        printf("\e[1;32mCuWSActionReader.decodeMessage: posting update %s on listener %p\e[0m\n",
               res.toString().c_str(), (*it));
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
    qDebug() << __PRETTY_FUNCTION__ <<  "data" << data << " error " << reply->errorString();
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
            qDebug() << __PRETTY_FUNCTION__ << requrl;
            QString key = requrl.section('/', -1);
            d->source_configuration.add(key, QString(data).remove("\""));
            if(d->source_configuration.isComplete()) {
                CuData conf = d->source_configuration.toCuData();
                conf["src"] = d->tsrc.getName();
                conf["err"] = false; //d->source_configuration.error();
                if(d->proto_helper_i)
                    conf["data_format_str"] = d->proto_helper_i->dataFormatToStr(conf["data_format"].toInt());
                conf["msg"] = d->source_configuration.errorMessage().toStdString();
                conf["type"] = "property";

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
    qDebug() << __PRETTY_FUNCTION__ << "http_url is " << d->http_url;
    if(d->http_url.isEmpty()) {
        // communicate over websocket only
        QString msg = QString("SUBSCRIBE %1").arg(url_s);
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
                qDebug() << __PRETTY_FUNCTION__ << "Sending " << config_r.url().toString();
                d->networkAccessManager->get(config_r);
            }
        }

        // 2. subscribe to events

        QNetworkRequest subscribe_request(d->http_url + url_s);
        //    subscribe_request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        //    pgreen2tmp("CuWSActionReader::start: subscribing to \"%s\"", qstoc(subscribe_request.url().toString()));
        d->networkAccessManager->sendCustomRequest(subscribe_request, QByteArray("SUBSCRIBE"));
        //    qDebug() << __PRETTY_FUNCTION__ << "sendCustomRequest sent: reply ?" << r->errorString() << r->readAll();
    }
}

void CuWSActionReader::stop()
{
    if(!d->exit)
        d->exit = true;

    CuData tok = getToken();
    QString url_s = QString::fromStdString(tok["src"].toString());
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
}

