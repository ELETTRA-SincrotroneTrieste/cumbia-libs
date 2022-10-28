#include "cudataserializer.h"
#include <string.h>
#include <stdlib.h>
#include <cuvariant.h>
#include <sys/types.h>

union {
    uint32_t i;
    unsigned char c[4];
} aint = { 0x80000000 };

static uint8_t be = aint.c[0];

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
    re.stat = be;
    strncpy(re.src, da.s("src").c_str(), 127);
    if(da.B("err")) re.stat |= 0x1;
    if(da.containsKey("E")) re.stat |= 0x2;
    std::string msg;
    if(da.containsKey("msg")) msg = da.s("msg");
    double ts = da["timestamp_us"].toDouble();
    memcpy(&re.timestamp, &ts, sizeof(uint64_t));
    if(da.containsKey("value")) {
        const CuVariant& v = da["value"];
        re.type = static_cast<uint8_t>(v.getType());
        re.format = static_cast<uint8_t>(v.getFormat());

        switch(re.type) {
        case CuVariant::Double:
            buf = data_init_t<double>(&re, v.getSize(), msg.length());
            buf = serialize_t<double>(buf, &re, v.toDoubleP());
            break;
        case CuVariant::LongDouble:
            buf = data_init_t<long double>(&re, v.getSize(), msg.length());
            buf = serialize_t<long double>(buf, &re, v.toLongDoubleP());
            break;
        case CuVariant::Float:
            buf = data_init_t<float>(&re, v.getSize(), msg.length());
            buf = serialize_t<float>(buf, &re, v.toFloatP());
            break;
        case CuVariant::Short:
            buf = data_init_t<short>(&re, v.getSize(), msg.length());
            buf = serialize_t<short>(buf, &re, v.toShortP());
            break;
        case CuVariant::UShort:
            buf = data_init_t<unsigned short>(&re, v.getSize(), msg.length());
            buf = serialize_t<unsigned short>(buf, &re, v.toUShortP());
            break;
        case CuVariant::Int:
            buf = data_init_t<int>(&re, v.getSize(), msg.length());
            buf = serialize_t<int>(buf, &re, v.toIntP());
            break;
        case CuVariant::LongInt:
            buf = data_init_t<long int>(&re, v.getSize(), msg.length());
            buf = serialize_t<long int>(buf, &re, v.toLongIntP());
            break;
        case CuVariant::LongUInt:
            buf = data_init_t<long unsigned int>(&re, v.getSize(), msg.length());
            buf = serialize_t<long unsigned int>(buf, &re, v.toULongIntP());
            break;
        case CuVariant::LongLongInt:
            buf = data_init_t<long long int>(&re, v.getSize(), msg.length());
            buf = serialize_t<long long int>(buf, &re, v.toLongLongIntP());
            break;
        case CuVariant::LongLongUInt:
            buf = data_init_t<unsigned long long int>(&re, v.getSize(), msg.length());
            buf = serialize_t<unsigned long long int>(buf, &re, v.toULongLongIntP());
            break;
        case CuVariant::Char:
            buf = data_init_t<char>(&re, v.getSize(), msg.length());
            buf = serialize_t<char>(buf, &re, v.toCharP());
            break;
        case CuVariant::UChar:
            buf = data_init_t<unsigned char>(&re, v.getSize(), msg.length());
            buf = serialize_t<unsigned char>(buf, &re, v.toUCharP());
            break;
        case CuVariant::Boolean:
            buf = data_init_t<bool>(&re, v.getSize(), msg.length());
            buf = serialize_t<bool>(buf, &re, v.toBoolP());
            break;
        case CuVariant::String:
            buf = data_s_init(&re, v.to_C_charP(), v.getSize(), msg.length());
            buf = serialize_string(buf, &re, v.to_C_charP(), v.getSize());
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
    } // if(da.containsKey("value"))
    if(!buf) { // no value key
        buf = data_init_t<double>(&re, 0, msg.length());
    }
    return buf;
}

uint32_t CuDataSerializer::size(const char *data) const {
    return ((uint32_t *) (data + roffsets.size))[0];
}

uint32_t CuDataSerializer::data_size(const char *data) const {
    return ((uint32_t *) (data + roffsets.datasiz))[0];
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

const char *CuDataSerializer::msg_ptr(const char *data) const {
    return (this->size(data) > sizeof(struct repr) + sizeof(char) * data_size(data) ) ?
                data + sizeof(struct repr) + sizeof(char) * data_size(data) : nullptr;
}

std::string CuDataSerializer::message(const char *data) const {
    std::string m;
    const char *s = msg_ptr(data);
    if(s)
        m = std::string(s, s + msg_len(data));
    return m;
}

uint32_t CuDataSerializer::msg_len(const char *data) const {
    return ((uint32_t *) (data + roffsets.msgsiz))[0];
}

/*! \brief deserialize from binary data
 *
 * struct repr version check
 *
 * extracted information:
 * *src*: the source of data, at most CUDATASER_MAXSRCLEN  long
 * *dt*: data type, see CuVariant::DataType, f.e. CuVariant::Scalar, CuVariant::Vector
 * *df*: data format see CuVariant::DataFormat, f.e. CuVariant::Double, CuVariant::String
 * *err*: the error flag. if true an error message is normally available. You can check
 *      if struct repr.msgsiz is nonzero in any case
 * *timestamp_us*: timestamp as double: seconds.microseconds
 * *timestamp_ms*: timestamp as double: milliseconds.microseconds
 * *value*: if present, check the data size in *struct repr.datasiz*. Data starts after
 *         sizeof(struct repr) bytes in the binary data and is long *struct repr.datasiz* bytes
 *
 * Data presence is indicated by a non zero *struct repr.datasiz* and a non null *data_ptr*
 * return value.
 *
 * Data may optionally be followed by a message, unless *err* is true. In the latter case, an
 * error message is always foreseen.
 *
 * *struct repr. msgsiz* indicates the size of the message, in bytes, that starts *after the
 * data bulk*. You can check if a message is present either testing for msgsiz > 0
 * or a non null *msg_ptr*.
 *
 * Convenience functions help accessing data:
 *
 * *msg_ptr* provides a pointer to the first byte of the message string, long *struct repr. msgsiz*
 *
*/
CuData CuDataSerializer::deserialize(const char *data, size_t len) const {
    CuData d;
    if(len < sizeof(uint32_t) + sizeof(uint8_t)) {
        d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize length of data ") + std::to_string(len) + " is too short. serializer version: " + std::to_string(VERSION));
        perr("%s", d.s("msg").c_str());
    }
    else if(version(data) == VERSION) {
        uint32_t siz = this->size(data);
        if(siz >= sizeof(struct repr)) {
            struct repr *re = (struct repr *) data;
            d["src"] = std::string(re->src);
            d["dt"] = re->type;
            d["df"] = re->format;
            d["err"] = re->stat & 0x1;
            re->stat & 0x2 ? d["E"] = "event" : d["mode"] = "P";
            double ts;
            memcpy(&ts, &re->timestamp, sizeof(re->timestamp));
            d["timestamp_us"] = ts; // timestamp is seconds.microseconds
            d["timestamp_ms"] = ts * 1000.0; // so we want milliseconds.microsecs
            uint32_t *p_dsiz = (uint32_t *) (data + roffsets.datasiz); // data size in bytes
            if(*p_dsiz > 0 && siz == sizeof(struct repr) + (*p_dsiz) ) {
                CuVariant::DataType t = static_cast<CuVariant::DataType>(re->type);
                CuVariant::DataFormat f = static_cast<CuVariant::DataFormat>(re->format);
                switch(t) {
                case CuVariant::Double:
                    d["value"] = deserialize_data_t<double>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::LongDouble:
                    d["value"] = deserialize_data_t<long double>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::Float:
                    d["value"] = deserialize_data_t<float>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::Short:
                    d["value"] = deserialize_data_t<short>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::UShort:
                    d["value"] = deserialize_data_t<unsigned short>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::Int:
                    d["value"] = deserialize_data_t<int>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::LongInt:
                    d["value"] = deserialize_data_t<long int>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::LongUInt:
                    d["value"] = deserialize_data_t<unsigned long>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::LongLongInt:
                    d["value"] = deserialize_data_t<long long int>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::LongLongUInt:
                    d["value"] = deserialize_data_t<unsigned long long>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::Char:
                    d["value"] = deserialize_data_t<char>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::UChar:
                    d["value"] = deserialize_data_t<unsigned char>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::Boolean:
                    d["value"] = deserialize_data_t<unsigned char>(data_ptr(data), *p_dsiz, t, f);
                    break;
                case CuVariant::String:
                    d["value"] = deserialize_string(data_ptr(data), *p_dsiz, f);
                    break;
                case CuVariant::VoidPtr:
                    d["value"] = deserialize_data_t<void *>(data_ptr(data), *p_dsiz, t, f);
                    break;
                default:
                    d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize unsupported data type " + std::to_string(re->type)));
                    perr("%s", d.s("msg").c_str());
                    break;
                }
            }
            else if(*p_dsiz > 0) {
                d.set("err", true).set("msg", std::string("CuDataSerializer::deserialize: buffer size ") + std::to_string(siz) + " < " + std::to_string(sizeof(struct repr)) + " (metadata size) + " + std::to_string(*p_dsiz) + " (expected bytes of data)");
                perr("%s", d.s("msg").c_str());
            }

            if(msg_ptr(data) != nullptr) {
                d["msg"] = message(data);
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
char *CuDataSerializer::data_init_t(repr *re, size_t datalen, size_t msglen) const {
    char *databuf = nullptr;
    re->datasiz = datalen * sizeof(T);
    re->msgsiz = msglen * sizeof(char);
    re->size = sizeof(struct repr) + re->datasiz + re->msgsiz;
    databuf = (char *) malloc(re->size);
    memcpy(databuf, re, sizeof(struct repr));
    return databuf;
}

template<typename T>
char *CuDataSerializer::serialize_t(char *databuf, struct repr *re, T *p) const {
    memcpy(databuf + sizeof(struct repr), p, re->datasiz);
    printf("CuDataSerializer.serialize_t <T>: tot siz \e[1;32m%u\e[0m data siz \e[0;36m%u\e[0m\n", re->size, re->datasiz);
    return databuf;
}

char *CuDataSerializer::data_s_init(repr *re, char **p, size_t len, size_t msglen) const {
    char *databuf = nullptr;
    re->datasiz = 0;
    // calculate necessary size
    for(size_t i = 0; i < len; i++)
        re->datasiz += (strlen(p[i]) + 1) * sizeof(char);
    re->msgsiz = msglen * sizeof(char);
    re->size = sizeof(struct repr) + re->datasiz + re->msgsiz;
    memcpy(databuf, re, sizeof(struct repr));
    return databuf;
}

char *CuDataSerializer::serialize_string(char *databuf, struct repr *re, char **p, size_t len) const {
    size_t j = 0;
    char *datap = databuf + sizeof(struct repr);
    memset(datap, 0, re->datasiz);
    for(size_t i = 0; i < len; i++) {
        strncpy(datap + j, p[i], re->datasiz - j);
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
