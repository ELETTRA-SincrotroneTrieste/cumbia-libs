#include "cumbiawsworld.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <cudata.h>
#include <QStringList>
#include <QJsonArray>
#include <QtDebug>
#include <math.h>

CumbiaWSWorld::CumbiaWSWorld()
{
    m_src_patterns.push_back("ws[s]{0,1}://.+");
}

void CumbiaWSWorld::setSrcPatterns(const std::vector<std::string> &pat_regex)
{
    m_src_patterns = pat_regex;
}

std::vector<std::string> CumbiaWSWorld::srcPatterns() const
{
    return m_src_patterns;
}

bool CumbiaWSWorld::source_valid(const std::string &)
{
    return true;
}

bool CumbiaWSWorld::json_decode(const QJsonDocument &json, CuData &res)
{
    if(json.isNull()) {
        res[TTT::Err] = true;  // res["err"]
        res[TTT::Message] = "CuWSActionReader.decodeMessage: invalid json document";  // res["msg"]
    }
    else {
        const QJsonObject data_o = json["data"].toObject();
        if(!data_o.isEmpty()) {
            QStringList keys = data_o.keys();

            // NOTE
            // these are the keys storing values that necessary need to be converted to int
            QStringList i_keys = QStringList() << "s" << "q" << "writable" << "dt" << "df";
            QStringList special_keys = QStringList() << "timestamp" << "timestamp_us" << "timestamp_ms" << "err" << "error" << "msg";

            foreach(const QString &k, keys + special_keys ) {
                const QJsonValue &v = data_o[k]; // const version, data_o is const
                const std::string &c = k.toStdString();
                if(!i_keys.contains(k) && v.isArray()) {
                    QJsonArray jarr = v.toArray();
                    // decide type
                    if(jarr.size() > 0 && jarr.at(0).isDouble()) {
                        // all type of ints are saved as double in Json
                        std::vector<double> vd;
                        for(int i = 0; i < jarr.size(); i++) {
                            QJsonValue ithval = jarr.at(i);
                            vd.push_back(ithval.toDouble());
                        }
                        res[c] = vd;
                    }
                    else if(jarr.size() > 0 && jarr.at(0).isBool()) {
                        std::vector<bool> vb;
                        for(int i = 0; i < jarr.size(); i++) {
                            QJsonValue ithval = jarr.at(i);
                            vb.push_back(ithval.toBool());
                        }
                        res[c] = vb;
                    }
                    else if(jarr.size() > 0 && jarr.at(0).isString()) {
                        std::vector<std::string> vs;
                        for(int i = 0; i < jarr.size(); i++) {
                            QJsonValue ithval = jarr.at(i);
                            vs.push_back(ithval.toString().toStdString());
                        }
                        res[c] = vs;
                    }
                }
                else if(!i_keys.contains(k)) { // scalar
                    if(v.isString())
                        res[c] = v.toString().toStdString();
                    else if(v.isBool())
                        res[c] = v.toBool();
                    else if(v.isDouble())
                        res[c] = v.toDouble();
                }
            }

            // timestamp
            char *endptr;
            double ts_us = -1.0f;
            if(data_o.contains("timestamp"))
                ts_us = strtod(data_o["timestamp"].toString().toStdString().c_str(), &endptr); // !cudata
            else if(data_o.contains("timestamp_us"))  // !cudata
                ts_us = data_o["timestamp_us"].toDouble(); // !cudata
            if(ts_us >= 0)
                res[TTT::Time_us] = ts_us;  // res["timestamp_us"]

            // timestamp millis
            if(data_o.contains("timestamp_ms")) // timestamp_ms converted to long int
                res[TTT::Time_ms] =
                    data_o["timestamp_ms"].toDouble();  // !cudata
            else if(ts_us >= 0)
                res[TTT::Time_ms] = floor(ts_us) * 1000.0 + (ts_us - floor(ts_us)) * 10e6 / 1000.0;

            if(data_o.contains("error")) {
                res[TTT::Message] =
                    data_o["error"].toString().toStdString();  // !cudata
                res[TTT::Err] =
                    data_o["error"].toString().size() > 0;  // !cudata
            }
            else {
                res[TTT::Err] =
                    data_o["err"].toBool();  // !cudata
                res[TTT::Message] =
                    data_o["msg"].toString().toStdString();  // !cudata
            }

            //
            // to int
            foreach(const QString& k, i_keys)
                if(data_o.contains(k))
                    res[k.toStdString()] = data_o[k].toInt();
        }
    }
    return !json.isNull();
}
