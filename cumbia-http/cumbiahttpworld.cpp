#include "cumbiahttpworld.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <cudata.h>
#include <QStringList>
#include <QJsonArray>
#include <QtDebug>
#include <math.h>
#include <sys/time.h>

CumbiaHTTPWorld::CumbiaHTTPWorld() {
    m_src_patterns.push_back("http[s]{0,1}://.+");
}

void CumbiaHTTPWorld::setSrcPatterns(const std::vector<std::string> &pat_regex) {
    m_src_patterns = pat_regex;
}

std::vector<std::string> CumbiaHTTPWorld::srcPatterns() const {
    return m_src_patterns;
}

bool CumbiaHTTPWorld::source_valid(const std::string &) {
    return true;
}

bool CumbiaHTTPWorld::json_decode(const QJsonValue &v, CuData &out) const {
    m_json_decode(v, out);
    return !v.isNull();
}

bool CumbiaHTTPWorld::json_decode(const QByteArray &ba, std::list<CuData> &out) const {
    QJsonParseError pe;
    QJsonDocument json = QJsonDocument::fromJson(ba, &pe);
    if(pe.error != QJsonParseError::NoError) {
        perr("CumbiaHTTPWorld::json_decode: parse error: %s", qstoc(pe.errorString()));
        //        res["err"] = true;
        //        res["msg"] = "CumbiaHTTPWorld.decodeMessage: invalid json document";
    }
    else {
        const QJsonArray a = json.array();
        for(int i = 0; i < a.size(); i++) {
            CuData r;
            m_json_decode(a.at(i), r);
            out.push_back(r);
        }
    }
    return !json.isNull();
}

void CumbiaHTTPWorld::m_json_decode(const QJsonValue &data_v, CuData &res) const
{
    QJsonObject data_o = data_v.toObject();
    QStringList keys = data_o.keys();
    // value keys must be converted to the original type: Json converts all numbers to double
    // value_type key stores the CuVariant data type to facilitate conversion
    QStringList value_keys = QStringList() << "value" << "w_value";

    // NOTE
    // these are the keys storing values that necessary need to be converted to int
    QStringList i_keys = QStringList() << "state" << "quality" << "writable" << "data_type" << "data_format";
    QStringList special_keys = QStringList() << "timestamp" << "timestamp_us" << "timestamp_ms" << "err" << "error" << "msg";

    foreach(const QString &k, keys + special_keys ) {
        const QJsonValue &v = data_o[k]; // const version, data_o is const
        const std::string &c = k.toStdString();
        if(!i_keys.contains(k) && v.isArray()) {
            QJsonArray jarr = v.toArray();
            // decide type
            if(jarr.size() > 0 && jarr.at(0).isDouble() && !value_keys.contains(k)) {
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
        ts_us = strtod(data_o["timestamp"].toString().toStdString().c_str(), &endptr);
    else if(data_o.contains("timestamp_us"))
        ts_us = data_o["timestamp_us"].toDouble();
    if(ts_us > 0)
        res["timestamp_us"] = ts_us;

    // timestamp millis
    if(data_o.contains("timestamp_ms")) // timestamp_ms converted to long int
        res["timestamp_ms"] = data_o["timestamp_ms"].toDouble();
    else if(ts_us >= 0)
        res["timestamp_ms"] = floor(ts_us) * 1000.0 + (ts_us - floor(ts_us)) * 10e6 / 1000.0;

    res["err"] = data_o["err"].toBool();
    res["msg"] = data_o["msg"].toString().toStdString();
    //
    // to int
    foreach(const QString& k, i_keys)
        if(data_o.contains(k))
            res[k.toStdString()] = data_o[k].toInt();

    // value, w_value
    CuVariant::DataType t = static_cast<CuVariant::DataType>(data_o["value_type"].toDouble());
    foreach(const QString &k, value_keys) {
        std::string sk = k.toStdString();
        const QJsonValue &v = data_o[k];
        if(v.isArray()) {
            QJsonArray jarr = v.toArray();
            // decide type
            if(jarr.size() > 0 && jarr.at(0).isDouble()) {
                // all type of ints are saved as double in Json
                switch(t) {
                case CuVariant::Double: {
                    std::vector<double> vd;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vd.push_back(ithval.toDouble());
                    }
                    res[sk] = vd;
                }
                    break;
                case CuVariant::LongDouble: {
                    std::vector<long double> vd;
                    for(int i = 0; i < jarr.size(); i++) {
                        vd.push_back(static_cast<long double>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vd;
                }
                    break;
                case CuVariant::Int: {
                    std::vector<int> vi;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vi.push_back(static_cast<int>(ithval.toDouble()));
                    }
                    res[sk] = vi;
                }
                    break;
                case CuVariant::LongInt: {
                    std::vector<long int> vli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vli.push_back(static_cast<long int>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vli;
                    break;
                }
                case CuVariant::LongLongInt: {
                    std::vector<long long int> vlli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vlli.push_back(static_cast<long long int>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vlli;
                    break;
                }
                case CuVariant::LongLongUInt: {
                    std::vector<long long unsigned int> vulli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vulli.push_back(static_cast<long long unsigned int>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vulli;
                    break;
                }
                case CuVariant::LongUInt: {
                    std::vector<long unsigned int> vuli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vuli.push_back(static_cast<long unsigned int>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vuli;
                    break;
                }
                case CuVariant::UInt: {
                    std::vector<unsigned int> vui;
                    for(int i = 0; i < jarr.size(); i++) {
                        vui.push_back(static_cast<unsigned int>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vui;
                    break;
                }
                case CuVariant::UShort: {
                    std::vector<unsigned short> vus;
                    for(int i = 0; i < jarr.size(); i++) {
                        vus.push_back(static_cast<unsigned short>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vus;
                    break;
                }
                case CuVariant::Short: {
                    std::vector<short> vus;
                    for(int i = 0; i < jarr.size(); i++) {
                        vus.push_back(static_cast<short>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vus;
                    break;
                }
                case CuVariant::Float: {
                    std::vector<float> vf;
                    for(int i = 0; i < jarr.size(); i++) {
                        vf.push_back(static_cast<float>(jarr.at(i).toDouble()));
                    }
                    res[sk] = vf;
                    break;
                }
                case CuVariant::String:
                case CuVariant::TypeInvalid:
                case CuVariant::Boolean:
                case CuVariant::VoidPtr:
                case CuVariant::EndDataTypes:
                    break;
                }
            } // if(jarr.size() > 0 && jarr.at(0).isDouble())
        } // if(v.isArray())
        else {
            double dv = v.toDouble();
            switch(t) {
            case CuVariant::Double:
                res[sk] = dv;
                break;
            case CuVariant::LongDouble:
                res[sk] = static_cast<long double>(dv);
                break;
            case CuVariant::Int:
                res[sk] = static_cast<int>(dv);
                break;
            case CuVariant::LongInt:
                res[sk] = static_cast<long int>(dv);
                break;
            case CuVariant::LongLongInt:
                res[sk] = static_cast<long long int>(dv);
                break;
            case CuVariant::LongLongUInt:
                res[sk] = static_cast<long long unsigned >(dv);
                break;
            case CuVariant::LongUInt:
                res[sk] = static_cast<long unsigned >(dv);
                break;
            case CuVariant::UInt:
                res[sk] = static_cast<unsigned >(dv);
                break;
            case CuVariant::UShort:
                res[sk] = static_cast<unsigned short>(dv);
                break;
            case CuVariant::Short:
                res[sk] = static_cast<short>(dv);
                break;
            case CuVariant::Float:
                res[sk] = static_cast<float>(dv);
                break;
                // dealt with in first loop
            case CuVariant::String:
            case CuVariant::TypeInvalid:
            case CuVariant::Boolean:
            case CuVariant::VoidPtr:
            case CuVariant::EndDataTypes:
                break;
            }
        }
    } // foreach(const QString &k, value_keys)
}

QJsonObject CumbiaHTTPWorld::make_error(const QString &msg) const
{
    QJsonObject o;
    o["msg"] = msg;
    o["err"] = true;
    // timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
    o["timestamp_ms"] =  tv.tv_sec * 1000 + tv.tv_usec / 1000.0;
    o["timestamp_us"] = static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6;
    return o;
}

