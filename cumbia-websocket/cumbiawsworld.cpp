#include "cumbiawsworld.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <cudata.h>
#include <QStringList>
#include <QJsonArray>
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
                res["timestamp_ms"] = data_o["timestamp_ms"].toDouble();
            else if(ts_us >= 0)
                res["timestamp_ms"] = floor(ts_us) * 1000.0 + (ts_us - floor(ts_us)) * 10e6 / 1000.0;

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
                                                  << "label"<< "max" << "max_alarm"<< "max_dim_x"  << "max_dim_y" << "max_warning" << "min"
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
            keys = QStringList() << "state" << "quality" << "writable" << "data_type" << "data_format";
            foreach(const QString& k, keys)
                if(data_o.contains(k))
                    res[k.toStdString()] = data_o[k].toInt();
        }
    }
    return !json.isNull();
}
