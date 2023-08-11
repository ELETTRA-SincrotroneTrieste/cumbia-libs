#include "cumbiahttpworld.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <cudata.h>
#include <QStringList>
#include <QJsonArray>
#include <QtDebug>
#include <math.h>
#include <QElapsedTimer>
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

bool CumbiaHTTPWorld::json_simple_decode(const QByteArray &jba, CuData &out) const
{
    QJsonParseError pe;
    QJsonObject o;
    QJsonDocument json = QJsonDocument::fromJson(jba, &pe);
    if(pe.error != QJsonParseError::NoError) {
        out[CuDType::Err] = true;  // out["err"]
        out[CuDType::Message] = "CumbiaHTTPWorld::json_simple_decode: parse error: " + pe.errorString().toStdString() + " in \"" + jba.data() + "\"";  // out["msg"]
        perr("%s", out.s(CuDType::Message).c_str());  // out.s("msg")
    }
    else {
        if(json.isObject()) o = json.object();
        else if(json.isArray() && json.array().size() > 0) o = json.array().first().toObject();
        if(!o.isEmpty()) {
            foreach(const QString& k, o.keys()) {
                const QJsonValue& v = o.value(k);
                if(v.isBool()) out[k.toStdString()] = v.toBool(false);
                else if(v.isDouble()) out[k.toStdString()] = v.toDouble();
                else if(v.isString()) out[k.toStdString()] = o.value(k).toString().toStdString();
            }
        }
        else {
            out[CuDType::Err] = true;  // out["err"]
            out[CuDType::Message] = "CumbiaHTTPWorld::json_simple_decode: json \"" + std::string(jba.data()) + " is not an object";  // out["msg"]
            perr("%s", out.s(CuDType::Message).c_str());  // out.s("msg")
        }
    }
    return !o.isEmpty();
}

bool CumbiaHTTPWorld::request_reverse_eng(const QString &json, QMap<QString, QString>& map, QString& channel) const
{
    QJsonParseError pe;
    QJsonDocument jd = QJsonDocument::fromJson(json.toLatin1(), &pe);
    bool ok = (pe.error == QJsonParseError::NoError);
    if(!ok) {
        perr("CumbiaHTTPWorld::request_reverse_eng: parse error: %s", qstoc(pe.errorString()));
    }
    else {
        QJsonObject o = jd.object();
        if(o.contains("channel"))
            channel = o.value("channel").toString();
        if(o.contains("srcs")) {
            QJsonArray a = o.value("srcs").toArray();
            for(int i = 0; i < a.size(); i++) {
                const QJsonObject &jo = a[i].toObject();
                map[jo.value("src").toString()] = QJsonDocument(jo).toJson(QJsonDocument::Indented);  // !cudata
            }
        }
    }
    return ok;
}

bool CumbiaHTTPWorld::json_decode(const QByteArray &ba, std::list<CuData> &out) const {
    pretty_pri("data %s", ba.data());
    QJsonParseError pe;
    QJsonDocument json = QJsonDocument::fromJson(ba, &pe);
    if(pe.error != QJsonParseError::NoError) {
        perr("CumbiaHTTPWorld::json_decode: parse error: %s", qstoc(pe.errorString()));
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

void CumbiaHTTPWorld::m_json_decode(const QJsonValue &data_v, CuData &res) const {
    QElapsedTimer elapt;
    elapt.start();
    const QJsonObject &data_o = data_v.toObject();
    // NOTE
    // these are the keys storing values that necessary need to be converted to int
    CuDTypeUtils dtu;
    const int M = CuDType::MaxDataKey;
    std::vector<CuDType::Key> idx_keys;
    std::vector<std::string> str_keys;
    for(const QString& s : data_o.keys()) {
        if(s != "value" && s != "w_value") { // they have specific types
            CuDType::Key K = dtu.idx(s.toStdString().c_str());
            const QJsonValue &v = data_o[s]; // const version, data_o is const
            const std::string &c = s.toStdString();
            if(v.isArray()) {
                QJsonArray jarr = v.toArray();
                // decide type
                if(jarr.size() > 0 && jarr.at(0).isDouble()) {
                    // all type of ints are saved as double in Json
                    std::vector<double> vd;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vd.push_back(ithval.toDouble());
                    }
                    K != M ? res[K] = vd : res[c] = vd;
                }
                else if(jarr.size() > 0 && jarr.at(0).isBool()) {
                    std::vector<bool> vb;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vb.push_back(ithval.toBool());
                    }
                    K != M ? res[K] = vb : res[c] = vb;
                }
                else if(jarr.size() > 0 && jarr.at(0).isString()) {
                    std::vector<std::string> vs;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vs.push_back(ithval.toString().toStdString());
                    }
                    K != M ? res[K] = vs : res[c] = vs;
                }
            }
            else  { // scalar
                if(v.isString())
                    K != M ? res[K] = v.toString().toStdString() : res[c] = v.toString().toStdString();
                else if(v.isBool())
                    K != M ?  res[K] = v.toBool() : res[c] = v.toBool();
                else if(v.isDouble())
                    K != M ?  res[K] = v.toDouble() : res[c] = v.toDouble();
            }
        }
    }

    // timestamp management
    char *endptr;
    double ts_us = -1.0f;
    if(data_o.contains("timestamp"))
        ts_us = strtod(data_o["timestamp"].toString().toStdString().c_str(), &endptr);
    else if(data_o.contains("timestamp_us"))  // !cudata
        ts_us = data_o["timestamp_us"].toDouble();  // !cudata
    if(ts_us > 0)
        res[CuDType::Time_us] = ts_us;  // res["timestamp_us"]

    // timestamp millis
    if(data_o.contains("timestamp_ms")) // timestamp_ms converted to long int
        res[CuDType::Time_ms] = data_o["timestamp_ms"].toDouble();  // !cudata
    else if(ts_us >= 0)
        res[CuDType::Time_ms] = floor(ts_us) * 1000.0 + (ts_us - floor(ts_us)) * 10e6 / 1000.0;  // res["timestamp_ms"]

    // error management: err flag and message
    if(data_o.contains("err")) {  // data_o.contains("err") !cudata
        res[CuDType::Err] = data_o["err"].toBool();  // res["err"], data_o["err"] !cudata
    }
    else if(data_o.contains("error"))
        res[CuDType::Err] = data_o["error"].toBool();  // res["err"] !cudata
    if(data_o.contains("msg"))  // data_o.contains("msg")
        res[CuDType::Message] = data_o["msg"].toString().toStdString();  // res["msg"], data_o["msg"] !cudata


    // matrix data type ? will have dim_x and dim_y
    int dimx = data_o["dim_x"].toInt(0), dimy = data_o["dim_y"].toInt(0);  // !cudata
    bool matrix = dimx > 0 && dimy > 0;

    // value, w_value vt: value type
    CuVariant::DataType t = static_cast<CuVariant::DataType>(data_o["vt"].toDouble());
    foreach(const CuDType::Key& K, QList<CuDType::Key>() << CuDType::Value << CuDType::WriteValue) {
        const QJsonValue &v = data_o[K == CuDType::Value ? "value" : "w_value"];
        if(v.isArray()) {
            QJsonArray jarr = v.toArray();
            // decide type
            if(jarr.size() > 0) {
                // all type of ints are saved as double in Json
                switch(t) {
                case CuVariant::Double: {
                    std::vector<double> vd;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vd.push_back(ithval.toDouble());
                    }
                    if(!matrix) res[K] = vd;
                    else res[K] = CuVariant(vd, dimx, dimy);
                } break;
                case CuVariant::LongDouble: {
                    std::vector<long double> vld;
                    for(int i = 0; i < jarr.size(); i++) {
                        vld.push_back(static_cast<long double>(jarr.at(i).toDouble()));
                    }
                    if(!matrix) res[K] = vld;
                    else res[K] = CuVariant(vld, dimx, dimy);
                }  break;
                case CuVariant::Int: {
                    std::vector<int> vi;
                    for(int i = 0; i < jarr.size(); i++) {
                        QJsonValue ithval = jarr.at(i);
                        vi.push_back(static_cast<int>(ithval.toInt()));
                    }
                    if(!matrix) res[K] = vi;
                    else  res[K] = CuVariant(vi, dimx, dimy);
                } break;
                case CuVariant::LongInt: {
                    std::vector<long int> vli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vli.push_back(static_cast<long int>(jarr.at(i).toVariant().toLongLong()));
                    }
                    if(!matrix) res[K] = vli;
                    else  res[K] = CuVariant(vli, dimx, dimy);
                }  break;
                case CuVariant::LongLongInt: {
                    std::vector<long long int> vlli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vlli.push_back(static_cast<long long int>(jarr.at(i).toVariant().toLongLong()));
                    }                    
                    if(!matrix) res[K] = vlli;
                    else res[K] = CuVariant(vlli, dimx, dimy);
                } break;
                case CuVariant::LongLongUInt: {
                    std::vector<long long unsigned int> vulli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vulli.push_back(static_cast<long long unsigned int>(jarr.at(i).toVariant().toULongLong()));
                    }
                    if(!matrix) res[K] = vulli;
                    else res[K] = CuVariant(vulli, dimx, dimy);
                } break;
                case CuVariant::LongUInt: {
                    std::vector<long unsigned int> vuli;
                    for(int i = 0; i < jarr.size(); i++) {
                        vuli.push_back(static_cast<long unsigned int>(jarr.at(i).toVariant().toULongLong()));
                    }
                    if(!matrix)res[K] = vuli ;
                    else  res[K] = CuVariant(vuli, dimx, dimy);
                } break;
                case CuVariant::UInt: {
                    std::vector<unsigned int> vui;
                    for(int i = 0; i < jarr.size(); i++) {
                        vui.push_back(static_cast<unsigned int>(jarr.at(i).toVariant().toUInt()));
                    }
                    if(!matrix) res[K] = vui ;
                    else res[K] =  CuVariant(vui, dimx, dimy);
                }  break;
                case CuVariant::UShort: {
                    std::vector<unsigned short> vus;
                    for(int i = 0; i < jarr.size(); i++) {
                        vus.push_back(static_cast<unsigned short>(jarr.at(i).toVariant().toUInt()));
                    }
                    if(!matrix) res[K] = vus;
                    else  res[K] = CuVariant(vus, dimx, dimy);
                }  break;
                case CuVariant::Short: {
                    std::vector<short> vs;
                    for(int i = 0; i < jarr.size(); i++) {
                        vs.push_back(static_cast<short>(jarr.at(i).toVariant().toInt()));
                    }
                    if(!matrix) res[K] = vs;
                    else res[K] = CuVariant(vs, dimx, dimy);
                } break;
                case CuVariant::Char: {
                    std::vector<char> vc;
                    for(int i = 0; i < jarr.size(); i++) {
                        vc.push_back(static_cast<char>(jarr.at(i).toVariant().toChar().toLatin1()));
                    }
                    if(!matrix)  res[K] = vc;
                    else res[K] = CuVariant(vc, dimx, dimy);
                } break;
                case CuVariant::UChar: {
                    std::vector<unsigned char> vuc;
                    for(int i = 0; i < jarr.size(); i++) {
                        vuc.push_back(static_cast<unsigned char>(jarr.at(i).toVariant().toUInt()));
                    }
                    if(!matrix) res[K] = vuc;
                    else res[K] = CuVariant(vuc, dimx, dimy);
                } break;
                case CuVariant::Float: {
                    std::vector<float> vf;
                    for(int i = 0; i < jarr.size(); i++) {
                        vf.push_back(static_cast<float>(jarr.at(i).toDouble()));
                    }
                    if(!matrix) res[K] = vf ;
                    else res[K] = CuVariant(vf, dimx, dimy);
                } break;
                case CuVariant::String: {
                    std::vector<std::string> vs;
                    for(int i = 0; i < jarr.size(); i++) {
                        vs.push_back(jarr.at(i).toString().toStdString());
                    }
                    if(!matrix)   res[K] = vs;
                    else  res[K] = CuVariant(vs, dimx, dimy);
                } break;
                case CuVariant::Boolean: {
                    std::vector<bool> vb;
                    for(int i = 0; i < jarr.size(); i++) {
                        vb.push_back(static_cast<bool>(jarr.at(i).toBool()));
                    }
                    if(!matrix) res[K];
                    else  res[K] = CuVariant(vb, dimx, dimy);
                } break;
                case CuVariant::VoidPtr:
                case CuVariant::TypeInvalid:
                case CuVariant::EndVariantTypes:
                case CuVariant::EndDataTypes:
                    break;
                }
            } // if(jarr.size() > 0 && jarr.at(0).isDouble())
            } // if(v.isArray())
        else {
            QVariant qv = v.toVariant();
            switch(t) {
            case CuVariant::Double:
                res[K] =  qv.toDouble();
                break;
            case CuVariant::LongDouble:
                res[K] =  static_cast<long double>(qv.toDouble());
                break;
            case CuVariant::Int:
                res[K] = static_cast<int>(qv.toInt());
                break;
            case CuVariant::LongInt:
                res[K] =  static_cast<long int>(qv.toLongLong());
                break;
            case CuVariant::LongLongInt:
                res[K] = static_cast<long long int>(qv.toLongLong());
                break;
            case CuVariant::LongLongUInt:
                res[K] =  static_cast<long long unsigned >(qv.toULongLong());
                break;
            case CuVariant::LongUInt:
                res[K] = static_cast<long unsigned >(qv.toULongLong());
                break;
            case CuVariant::UInt:
                res[K] = static_cast<unsigned >(qv.toUInt());
                break;
            case CuVariant::UShort:
                res[K] = static_cast<unsigned short>(qv.toUInt());
                break;
            case CuVariant::Short:
                res[K] = static_cast<short>(qv.toInt());
                break;
            case CuVariant::Float:
                res[K] = static_cast<float>(qv.toFloat());
                break;
            case CuVariant::Char:
                res[K] = qv.toChar().toLatin1();
                break;
            case CuVariant::UChar:
                res[K] = static_cast<unsigned char>(qv.toUInt());
                break;
                // dealt with in first loop
            case CuVariant::String:
                res[K] = qv.toString().toStdString();
                break;
            case CuVariant::Boolean:
                res[K] =  qv.toBool();
                break;
            case CuVariant::TypeInvalid:
            case CuVariant::VoidPtr:
            case CuVariant::EndVariantTypes:
            case CuVariant::EndDataTypes:
                break;
            }
        }
    } // foreach(const QString &k, value_keys)
//    printf("%s \e[1;36mtook %lldms\e[0m to decode %s\n", __PRETTY_FUNCTION__, elapt.elapsed(), datos(res));
}

QJsonObject CumbiaHTTPWorld::make_error(const QString &msg) const
{
    QJsonObject o;
    o["msg"] = msg;  // o["msg"]
    o["err"] = true;  // o["err"]
    // timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
    o["timestamp_ms"] =  tv.tv_sec * 1000 + tv.tv_usec / 1000.0;  // o["timestamp_ms"] !cudata
    o["timestamp_us"] = static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1e-6;  // o["timestamp_us"] !cudata
    return o;
}

QByteArray CumbiaHTTPWorld::get_http_payload(const QByteArray &buf) const
{
    int i = buf.indexOf("\\r\\n\\r\\n");
    if(i > -1)
        return buf.mid(i+4);
    else if((i = buf.indexOf("\\n\\n")) > -1)
        return buf.mid(i + 2);
    return QByteArray();
}

