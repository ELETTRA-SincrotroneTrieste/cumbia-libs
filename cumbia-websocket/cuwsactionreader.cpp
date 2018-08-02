#include "cuwsactionreader.h"
#include "cumbiawebsocket.h"
#include "cuwsactionfactoryservice.h"
#include "protocol/protocolhelper_i.h"
#include "protocolhelpers.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <curandomgenactivity.h>
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

QStringList WSSourceConfiguration::keys() const
{
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
    CumbiaWebSocket *cumbia_ws;
    bool exit;
    CuData property_d, value_d;
    QNetworkAccessManager *networkAccessManager;
    WSSourceConfiguration source_configuration;
    ProtocolHelpers *proto_helpers;
    ProtocolHelper_I *proto_helper_i;
};

CuWSActionReader::CuWSActionReader(const WSSource& src, CumbiaWebSocket *ct) : CuWSActionI()
{
    d = new CuWSActionReaderPrivate;
    d->tsrc = src;
    d->cumbia_ws = ct;
    d->exit = false;  // set to true by stop
    d->networkAccessManager = NULL;
    std::string proto = src.getProtocol(); // tango:// ?
    printf("\e[1;36mFOUND PROTOCOL \"%s\"\e[0m\n", proto.c_str());
    d->proto_helpers = new ProtocolHelpers();
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
    CuData res = getToken();
    if(json.isNull()) {
        res["err"] = true;
        res["msg"] = "CuWSActionReader.decodeMessage: invalid json document";
    }
    else {
        QJsonObject data_o = json["data"].toObject();
        if(!data_o.isEmpty()) {
            QJsonValue value = data_o["value"];
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
                    res["value"] = vd;
                }
                else if(jarr.size() > 0 && jarr.at(0).isBool()) {
                    std::vector<bool> vb;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vb.push_back(ithval.toBool());
                    }
                    res["value"] = vb;
                }
                else if(jarr.size() > 0 && jarr.at(0).isString()) {
                    std::vector<std::string> vs;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vs.push_back(ithval.toString().toStdString());
                    }
                    res["value"] = vs;
                }
            }
            else if(!value.isNull()){ // scalar
                res["data_format_str"] = std::string("scalar");
                if(value.isBool())
                    res["value"] = value.toBool();
                else if(value.isDouble())
                    res["value"] = value.toDouble();
                else if(value.isString())
                    res["value"] = value.toString().toStdString();
            }

            // timestamp
            char *endptr;
            double ts_us = strtod(data_o["timestamp"].toString().toStdString().c_str(), &endptr);

            res["timestamp_us"] = ts_us;
            res["timestamp_ms"] = static_cast<long int>(floor(ts_us) * 1000 + (ts_us - floor(ts_us)) * 10e6 / 1000);

            res["err"] = data_o["error"].toString().size() > 0;
            if(res["err"].toBool())
                res["msg"] = data_o["error"].toString().toStdString();
            else
                res["msg"] = "read ok";
        }


    }
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(res);
}


bool CuWSActionReader::exiting() const
{
    return d->exit;
}

void CuWSActionReader::onNetworkReplyFinished(QNetworkReply *reply)
{
    qDebug () << __FUNCTION__ << "the request was" << reply->request().url().toString();
    QByteArray data = reply->readAll();
    QString requrl = reply->request().url().toString();
    if(requrl == d->cumbia_ws->httpUrl() + QString::fromStdString(d->tsrc.getName())) {
        // we got the first reading of the source
        QJsonParseError jpe;
        QJsonDocument jd = QJsonDocument::fromJson(data, &jpe);
        if(!jd.isNull())
            decodeMessage(jd);
    }
    else {
        QString key = requrl.section('/', -1);
        d->source_configuration.add(key, QString(data).remove("\""));
        if(reply->error() != QNetworkReply::NoError)
            d->source_configuration.setError(reply->errorString());

        if(d->source_configuration.isComplete()) {
            CuData conf = d->source_configuration.toCuData();
            conf["src"] = d->tsrc.getName();
            conf["err"] = false; //d->source_configuration.error();
            if(d->proto_helper_i)
                conf["data_format_str"] = d->proto_helper_i->dataFormatToStr(conf["data_format"].toInt());
            conf["msg"] = d->source_configuration.errorMessage().toStdString();
            conf["type"] = "property";

            printf("\e[1;32mCuWSActionReader: configuration is complete: notifying with %s!\e[0m\n", conf.toString().c_str());
            for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
                printf("listener %p\n", *it);
                (*it)->onUpdate(conf);
            }
        }
    }
}

void CuWSActionReader::start()
{
    QString url_s = QString::fromStdString(d->tsrc.getName());
    d->networkAccessManager = new QNetworkAccessManager(this);

    // 1. get configuration parameters for the source
    //
    // for now, we must fetch each property with a separate get
    WSSourceConfiguration soco;
    QStringList keys = soco.keys();
    QNetworkRequest config_r;
    foreach(QString key, keys) {
        config_r.setUrl(QUrl(d->cumbia_ws->httpUrl() + url_s + "/" + key));
        d->networkAccessManager->get(config_r);
    }

    // 2. subscribe to events
    QNetworkRequest subscribe_request(d->cumbia_ws->httpUrl() + url_s);
//    subscribe_request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    pgreen2tmp("CuWSActionReader::start: subscribing to \"%s\"", qstoc(subscribe_request.url().toString()));
    d->networkAccessManager->sendCustomRequest(subscribe_request, QByteArray("SUBSCRIBE"));
    connect(d->networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyFinished(QNetworkReply*)));
}

void CuWSActionReader::stop()
{
    if(!d->exit)
        d->exit = true;

    CuData tok = getToken();
    QString url_s = QString::fromStdString(tok["src"].toString());
    // unsubscribe
    if(d->networkAccessManager) {
        QNetworkRequest unsubscribe_request;
        unsubscribe_request.setUrl(QUrl(d->cumbia_ws->httpUrl() + url_s));
//        unsubscribe_request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        pviolet2tmp("CuWSActionReader::stop: unsubscribing from \"%s\"", qstoc(unsubscribe_request.url().toString()));
        d->networkAccessManager->sendCustomRequest(unsubscribe_request, QByteArray("UNSUBSCRIBE"));

        delete d->networkAccessManager;
        d->networkAccessManager = NULL;
    }
}

