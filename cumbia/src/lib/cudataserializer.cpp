#include "cudataserializer.h"
#include <string.h>
#include <stdlib.h>
#include <cuvariant.h>
#include <sys/types.h>


/*
  struct repr {
    uint32_t size;

    uint8_t version;
    uint8_t type;
    uint8_t format;
    uint8_t stat; // 0|0|0|0|0|e|x|y  e: 0 LE, 1 BE, x = 1: event x = 0: poll y = 0 ok y = 1 ERR

    char src[MAXSRCLEN]; // 128 = 4 times 32

    uint64_t timestamp;
    uint32_t datasiz;
    uint32_t msgsiz;
    };
*/

char *CuDataSerializer::serialize(const CuData &da) const {
    char *buf = nullptr;
    struct repr re { 0 };
    re.version = VERSION;
    re.stat = 0;
    strncpy(re.src, da.s("src").c_str(), 127);
    if(da.B("err")) re.stat |= 0x1;
    double ts = da["timestamp_us"].toDouble();
    memcpy(&re.timestamp, &ts, sizeof(uint64_t));

    if(da.containsKey("value")) {
        const CuVariant& v = da["value"];
        re.type = static_cast<uint8_t>(v.getType());
        re.format = static_cast<uint8_t>(v.getFormat());

        switch(re.type) {
        case CuVariant::Double:
            buf = serialize_t<double>(&re, v.toDoubleP(), v.getSize());
            break;
        case CuVariant::LongDouble:
            buf = serialize_t<long double>(&re, v.toLongDoubleP(), v.getSize());
            break;
        case CuVariant::Float:
            buf = serialize_t<float>(&re, v.toFloatP(), v.getSize());
            break;
        case CuVariant::Short:
            buf = serialize_t<short>(&re, v.toShortP(), v.getSize());
            break;
        case CuVariant::UShort:
            buf = serialize_t<unsigned short>(&re, v.toUShortP(), v.getSize());
            break;
        case CuVariant::Int:
            buf = serialize_t<int>(&re, v.toIntP(), v.getSize());
            break;
        case CuVariant::LongInt:
            buf = serialize_t<long int>(&re, v.toLongIntP(), v.getSize());
            break;
        case CuVariant::LongUInt:
            buf = serialize_t<long unsigned int>(&re, v.toULongIntP(), v.getSize());
            break;
        case CuVariant::LongLongInt:
            buf = serialize_t<long long int>(&re, v.toLongLongIntP(), v.getSize());
            break;
        case CuVariant::LongLongUInt:
            buf = serialize_t<unsigned long long int>(&re, v.toULongLongIntP(), v.getSize());
            break;
        case CuVariant::Char:
            buf = serialize_t<char>(&re, v.toCharP(), v.getSize());
            break;
        case CuVariant::UChar:
            buf = serialize_t<unsigned char>(&re, v.toUCharP(), v.getSize());
            break;
        case CuVariant::Boolean:
            buf = serialize_t<bool>(&re, v.toBoolP(), v.getSize());
            break;
        case CuVariant::String:
            buf = serialize_string(&re, v.to_C_charP(), v.getSize());
            break;
        case CuVariant::VoidPtr:
        default:
            re.stat |= 0x1; // error
            const char *msg = "serialization type error";
            re.msgsiz = (strlen(msg) + 1) * sizeof(const char);
            buf = (char *) malloc(sizeof(struct repr) + re.msgsiz);
            memcpy(buf, &re, sizeof(struct repr));
            memcpy(buf + sizeof(struct repr), msg, re.msgsiz);
            break;
        }
    }
    return buf;
}

uint32_t CuDataSerializer::size(const char *data) const {
    return ((const uint32_t *) (data + roffsets.size))[0];
}

uint32_t CuDataSerializer::data_size(const char *data) const {
    return 0;
}

uint8_t CuDataSerializer::version(const char *data) const {
    return ((uint8_t *) (data + roffsets.version))[0];
}

const char *CuDataSerializer::data_ptr(const char *data) const {
    return (this->size(data) > sizeof(struct repr) ) ? (data + sizeof(struct repr) ) : nullptr;
}

const char *CuDataSerializer::src(const char *data) const {
    return data + roffsets.src;
}

CuData CuDataSerializer::deserialize(const char *data) const {
    CuData d;
    if(version(data) == VERSION) {
        uint32_t siz = this->size(data);
        if(siz >= sizeof(struct repr)) {
            struct repr *re = (struct repr *) data;
            d["src"] = std::string(re->src);
            d["dt"] = re->type;
            d["df"] = re->format;
            double ts;
            memcpy(&ts, &re->timestamp, sizeof(re->timestamp));
            d["timestamp_us"] = ts;
            d["timestamp_ms"] = ts/1000.0;
            uint32_t *p_dsiz = (uint32_t *) (data + roffsets.datasiz); // data size in bytes
            if(siz == sizeof(struct repr) + (*p_dsiz) ) {
                CuVariant::DataType t = static_cast<CuVariant::DataType>(re->type);
                CuVariant::DataFormat f = static_cast<CuVariant::DataFormat>(re->format);
                switch(t) {
                case CuVariant::Double:
                    d["value"] = deserialize_data_t<double>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::LongDouble:
                    d["value"] = std::move(deserialize_data_t<long double>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::Float:
                    d["value"] = std::move(deserialize_data_t<float>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::Short:
                    d["value"] = std::move(deserialize_data_t<short>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::UShort:
                    d["value"] = std::move(deserialize_data_t<unsigned short>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::Int:
                    d["value"] = std::move(deserialize_data_t<int>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::LongInt:
                    d["value"] = std::move(deserialize_data_t<long int>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::LongUInt:
                    d["value"] = std::move(deserialize_data_t<unsigned long>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::LongLongInt:
                    d["value"] = std::move(deserialize_data_t<long long int>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::LongLongUInt:
                    d["value"] = std::move(deserialize_data_t<unsigned long long>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::Char:
                    d["value"] = std::move(deserialize_data_t<char>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::UChar:
                    d["value"] = std::move(deserialize_data_t<unsigned char>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::Boolean:
                    d["value"] = std::move(deserialize_data_t<unsigned char>(data_ptr(data), *p_dsiz, t, f));
                    break;
                case CuVariant::String:
                    d["value"] = std::move(deserialize_string(data_ptr(data), *p_dsiz, f));
                    break;
                case CuVariant::VoidPtr:
                    d["value"] = std::move(deserialize_data_t<void *>(data_ptr(data), *p_dsiz, t, f));
                    break;
                default:
                    d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize unsupported data type " + std::to_string(re->type)));
                    perr("%s", d.s("msg").c_str());
                    break;
                }
            }
            else {
                d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize: buffer size ") + std::to_string(siz) + " < " + std::to_string(sizeof(struct repr)) + " (metadata size) + " + std::to_string(*p_dsiz) + " (expected bytes of data)");
                perr("%s", d.s("msg").c_str());
            }
        }
        else {
            d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize: received bytes " + std::to_string(siz) + " < medatadata size " + std::to_string(sizeof(struct repr)) ));
            perr("%s", d.s("msg").c_str());
        }
    } // version mismatch
    else {
        d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize version mismatch: ") + std::to_string(version(data)) + " expected " + std::to_string(VERSION));
        perr("%s", d.s("msg").c_str());
    }

    return d;
}


template<typename T>
char *CuDataSerializer::serialize_t(struct repr *re, T *p, size_t len) const {
    char *databuf = nullptr;
    re->datasiz = len * sizeof(T);
    re->size = sizeof(struct repr) + re->datasiz;
    databuf = (char *) malloc(sizeof(struct repr) + re->datasiz);
    memcpy(databuf, re, sizeof(struct repr));
    memcpy(databuf + sizeof(struct repr), p, re->datasiz);
    printf("CuDataSerializer.serialize_t <T>: tot siz \e[1;32m%u\e[0m data siz \e[0;36m%u\e[0m\n", re->size, re->datasiz);
    return databuf;
}

char *CuDataSerializer::serialize_string(struct repr *re, char **p, size_t len) const {
    char *databuf = nullptr;
    re->datasiz = 0;
    // calculate necessary size
    for(size_t i = 0; i < len; i++) {
        re->datasiz += (strlen(p[i]) + 1) * sizeof(char);
    }
    re->size = sizeof(struct repr) + re->datasiz;
    databuf = (char *) malloc(sizeof(struct repr) + re->datasiz);
    memcpy(databuf, re, sizeof(struct repr));
    size_t j = 0;
    char *datap = databuf + sizeof(struct repr);
    memset(datap, 0, re->datasiz);
    for(size_t i = 0; i < len; i++) {
        strncpy(datap + j, p[i], (strlen(p[i])));
        j += strlen(p[i]) + 1; // length of p[i] string + '\0'
    }
    return databuf;
}

// buf: pointer to the raw buffer (struct repr + actual data)
// datasiz: size of the actual data, in bytes
// t, f CuVariant DataType and Format to build the returned CuVariant
template<typename T>
CuVariant CuDataSerializer::deserialize_data_t(const char *data_ptr, size_t datasiz, CuVariant::DataType t, CuVariant::DataFormat f) const {
    size_t len = datasiz / sizeof(T); // length of vector (will be 1 if scalar)
    const T *pv = (const T *) data_ptr;
    return CuVariant(pv, len, f, t); // cumbia v1.5 constructor from raw data buffer and len (plus CuVariant DataType and Format)
}

CuVariant CuDataSerializer::deserialize_string(const char *dataptr, size_t datasiz, CuVariant::DataFormat f) const {
    if(f == CuVariant::Scalar) {
        printf("deserializing string scalar len %ld (expected 1)\n", datasiz);
        std::string s(dataptr, datasiz);
        return CuVariant(s);
    }
    else if(f == CuVariant::Vector) {
        std::vector<std::string> vs;
        const char *s = dataptr;
        printf("deserializing string VECTOR data siz %ld \n", datasiz);
        size_t j = 0;
        for(size_t i = 0; i < datasiz; i++) {
            if(s[j++] == '\0') {
                vs.push_back(std::string(s));
                printf("\e[1;33m\nsaving `%s`\e[0m\n", s);
                s = dataptr + i + 1;
                j = 0;
            }
        }
        printf("deserializing string vector siz %ld obtained vector of %ld strings\n", datasiz, vs.size());
        for(size_t i = 0; i < vs.size(); i++)
            printf("'%s'\n", vs[i].c_str());

        return CuVariant(vs);
    }
    return CuVariant();
}
