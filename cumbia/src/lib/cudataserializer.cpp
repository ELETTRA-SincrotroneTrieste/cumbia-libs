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
    printf("serializing %s data type %s\n", datos(da), da[TTT::Value].dataTypeStr(da[TTT::Value].getType()).c_str());  // da["value"], da["value"]
    char *buf = nullptr;
    struct repr re { 0 };
    re.version = VERSION;
    re.stat = be;
    strncpy(re.src, da.c_str(TTT::Src), 127);  // da.s("src")
    if(da.B(TTT::Err)) re.stat |= 0x1;  // da.B("err")
    if(da.containsKey(TTT::Event)) re.stat |= 0x2;  // da.containsKey("E")
    std::string msg = da.containsKey(TTT::Message) ? da.c_str(TTT::Message) : "";
    double ts = da[TTT::Time_us].toDouble();  // da["timestamp_us"]
    memcpy(&re.timestamp, &ts, sizeof(uint64_t));
    if(da.containsKey(TTT::Value)) {  // da.containsKey("value")
        const CuVariant& v = da[TTT::Value];  // da["value"]
        re.type = static_cast<uint8_t>(v.getType());
        re.format = static_cast<uint8_t>(v.getFormat());
        switch(re.type) {
        case CuVariant::Double:
            // matrix_ptr returns a static casted pointer to the matrix of the given type.
            buf = data_init_t<double>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<double>(buf, &re, v.toDoubleP());
            break;
        case CuVariant::LongDouble:
            buf = data_init_t<long double>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<long double>(buf, &re, v.toLongDoubleP());
            break;
        case CuVariant::Float:
            buf = data_init_t<float>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<float>(buf, &re, v.toFloatP());
            break;
        case CuVariant::Short:
            buf = data_init_t<short>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<short>(buf, &re, v.toShortP());
            break;
        case CuVariant::UShort:
            buf = data_init_t<unsigned short>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<unsigned short>(buf, &re, v.toUShortP());
            break;
        case CuVariant::Int:
            buf = data_init_t<int>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<int>(buf, &re, v.toIntP());
            break;
        case CuVariant::UInt:
            buf = data_init_t<unsigned int>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<unsigned int>(buf, &re, v.toUIntP());
            break;
        case CuVariant::LongInt:
            buf = data_init_t<long int>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<long int>(buf, &re, v.toLongIntP());
            break;
        case CuVariant::LongUInt:
            printf("Serializing long unsigned int\n");
            buf = data_init_t<long unsigned int>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<long unsigned int>(buf, &re, v.toULongIntP());
            break;
        case CuVariant::LongLongInt:
            buf = data_init_t<long long int>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<long long int>(buf, &re, v.toLongLongIntP());
            break;
        case CuVariant::LongLongUInt:
            buf = data_init_t<unsigned long long int>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<unsigned long long int>(buf, &re,  v.toULongLongIntP());
            break;
        case CuVariant::Char:
            buf = data_init_t<char>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<char>(buf, &re, v.toCharP());
            break;
        case CuVariant::UChar:
            buf = data_init_t<unsigned char>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<unsigned char>(buf, &re, v.toUCharP());
            break;
        case CuVariant::Boolean:
            buf = data_init_t<bool>(&re, v.getSize(), msg.length(), v.get_dim_x());
            buf = serialize_t<bool>(buf, &re, v.toBoolP());
            break;
        case CuVariant::String:
            if(re.format != CuVariant::Matrix) {
                buf = data_s_init(&re, v.to_C_charP(), v.getSize(), msg.length(), v.get_dim_x());
                buf = serialize_string(buf, &re, v.to_C_charP(), v.getSize());
            }
            else {
                char **m2char_v = std_string_matrix_to_char_p(v);
                buf = data_s_init(&re, m2char_v, v.getSize(), msg.length(), v.get_dim_x());
                buf = serialize_string(buf, &re, m2char_v, v.getSize());
                delete [] m2char_v;
            }
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
    if(!buf) // no value key, buf not initialized yet
        buf = data_init_t<double>(&re, 0, msg.length());

    if(msg.length() > 0) {
        char *som = buf + sizeof(struct repr) + data_size(buf);
        strncpy(som, msg.c_str(), msg.length());

    }
    return buf;
}

uint32_t CuDataSerializer::size(const char *data) const {
    return ((uint32_t *) (data + roffsets.size))[0];
}

uint32_t CuDataSerializer::data_size(const char *data) const {
    return ((uint32_t *) (data + roffsets.datasiz))[0];
}

uint32_t CuDataSerializer::rows(const char *data) const {
    return ((uint32_t*) (data + roffsets.rows))[0];
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
                data + sizeof(struct repr) + data_size(data) : nullptr;
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
        d.set(TTT::Err, true).set(TTT::Message, CuVariant("CuDataSerializer::deserialize length of data " + std::to_string(len) + " is too short. serializer version: " + std::to_string(VERSION)));  // set("err", true), set("msg", std::string("CuDataSerializer::deserialize length of data ")
        perr("%s", d.s(TTT::Message).c_str());  // d.s("msg")
    }
    else if(version(data) == VERSION) {
        uint32_t siz = this->size(data);
        if(siz >= sizeof(struct repr)) {
            struct repr *re = (struct repr *) data;
            d[TTT::Src] = CuVariant(re->src);  // d["src"]
            d[TTT::DataType] = re->type;  // d["dt"]
            d[TTT::DataFormat] = re->format;  // d["df"]
            if(re->stat & 0x1) d[TTT::Err] = true;  // d["err"]
            re->stat & 0x2 ? d["E"] = "event" : d[TTT::Mode] = "P";  // d["mode"]
            double ts;
            memcpy(&ts, &re->timestamp, sizeof(re->timestamp));
            d["timestamp_us"] = ts; // timestamp is seconds.microseconds
            d["timestamp_ms"] = ts * 1000.0; // so we want milliseconds.microsecs
            uint32_t *p_dsiz = (uint32_t *) (data + roffsets.datasiz); // data size in bytes
            uint32_t *p_rows = (uint32_t *) (data + roffsets.rows);
            if(*p_dsiz > 0 && siz >= sizeof(struct repr) + (*p_dsiz) ) {
                CuVariant::DataType t = static_cast<CuVariant::DataType>(re->type);
                CuVariant::DataFormat f = static_cast<CuVariant::DataFormat>(re->format);
                switch(t) {
                case CuVariant::Double:
                    d[TTT::Value] = deserialize_data_t<double>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::LongDouble:
                    d[TTT::Value] = deserialize_data_t<long double>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::Float:
                    d[TTT::Value] = deserialize_data_t<float>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::Short:
                    d[TTT::Value] = deserialize_data_t<short>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::UShort:
                    d[TTT::Value] = deserialize_data_t<unsigned short>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::Int:
                    d[TTT::Value] = deserialize_data_t<int>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::UInt:
                    d[TTT::Value] = deserialize_data_t<unsigned int>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::LongInt:
                    d[TTT::Value] = deserialize_data_t<long int>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::LongUInt:
                    d[TTT::Value] = deserialize_data_t<unsigned long>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::LongLongInt:
                    d[TTT::Value] = deserialize_data_t<long long int>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::LongLongUInt:
                    d[TTT::Value] = deserialize_data_t<unsigned long long>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::Char:
                    d[TTT::Value] = deserialize_data_t<char>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::UChar:
                    d[TTT::Value] = deserialize_data_t<unsigned char>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::Boolean:
                    d[TTT::Value] = deserialize_data_t<unsigned char>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::String:
                    d[TTT::Value] = deserialize_string(data_ptr(data), *p_dsiz, f, *p_rows);  // d["value"]
                    break;
                case CuVariant::VoidPtr:
                    d[TTT::Value] = deserialize_data_t<void *>(data_ptr(data), *p_dsiz, t, f, *p_rows);  // d["value"]
                    break;
                default:
                    d.set(TTT::Err, true).set(TTT::Message, CuVariant(std::string("CuDataSerializer::deserialize unsupported data type " + std::to_string(re->type))));  // set("err", true), set("msg", std::string("CuDataSerializer::deserialize unsupported data type " + std::to_string(re->type)
                    perr("%s", d.s(TTT::Message).c_str());  // d.s("msg")
                    break;
                }
            }
            else if(*p_dsiz > 0) {
                d.set(TTT::Err, true).set(TTT::Message, CuVariant(std::string("CuDataSerializer::deserialize: buffer size ") + std::to_string(siz) + " < " + std::to_string(sizeof(struct repr)) + " (metadata size) + " + std::to_string(*p_dsiz) + " (expected bytes of data)"));  // set("err", true), set("msg", std::string("CuDataSerializer::deserialize: buffer size ")
                perr("%s", d.s(TTT::Message).c_str());
            }

            if(msg_ptr(data) != nullptr) {
                d[TTT::Message] = CuVariant(message(data));
            }
        }
        else {
            d.set(TTT::Err, true).set(TTT::Message, CuVariant(std::string("CuDataSerializer::deserialize: received bytes " + std::to_string(siz) + " < medatadata size " + std::to_string(sizeof(struct repr)) )));  // set("err", true), set("msg", std::string("CuDataSerializer::deserialize: received bytes " + std::to_string(siz)
            perr("%s", d.s(TTT::Message).c_str());  // d.s("msg")
        }
    } // version mismatch
    else {
        d.set(TTT::Err, true).set(TTT::Message, CuVariant(std::string("CuDataSerializer::deserialize version mismatch: ") + std::to_string(version(data)) + " expected " + std::to_string(VERSION)));  // set("err", true), set("msg", std::string("CuDataSerializer::deserialize version mismatch: ")
        perr("%s", d.s(TTT::Message).c_str());  // d.s("msg")
    }

    return d;
}

template<typename T>
char *CuDataSerializer::data_init_t(repr *re, size_t datalen, size_t msglen, size_t nrows) const {
    char *databuf = nullptr;
    re->datasiz = datalen * sizeof(T);
    re->msgsiz = msglen * sizeof(char);
    re->rows = nrows;
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

char *CuDataSerializer::data_s_init(repr *re, char **p, size_t len, size_t msglen, size_t nrows) const {
    char *databuf = nullptr;
    re->datasiz = 0;
    re->rows = nrows;
    // calculate necessary size
    for(size_t i = 0; i < len; i++)
        re->datasiz += (strlen(p[i]) + 1) * sizeof(char);
    re->msgsiz = msglen * sizeof(char);
    re->size = sizeof(struct repr) + re->datasiz + re->msgsiz;
    databuf = (char *) malloc(re->size);
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
CuVariant CuDataSerializer::deserialize_data_t(const char *data_ptr, size_t datasiz, CuVariant::DataType t, CuVariant::DataFormat f, size_t rows) const {
    size_t len = datasiz / sizeof(T); // length of vector (will be 1 if scalar)
    const T *pv = (const T *) data_ptr;
    return CuVariant(pv, len, f, t, rows); // cumbia v1.5 constructor from raw data buffer and len (plus CuVariant DataType and Format)
}

CuVariant CuDataSerializer::deserialize_string(const char *dataptr, size_t datasiz, CuVariant::DataFormat f, size_t rows) const {
    if(f == CuVariant::Scalar) {
        printf("deserializing string scalar len %ld (expected 1)\n", datasiz);
        std::string s(dataptr, datasiz);
        return CuVariant(s);
    }
    else if(f == CuVariant::Vector || f == CuVariant::Matrix) {
        std::vector<std::string> vs;
        const char *s = dataptr;
        printf("deserializing string VECTOR data siz %ld \n", datasiz);
        size_t j = 0;
        for(size_t i = 0; i < datasiz; i++) {
            if(s[j++] == '\0') {
                vs.push_back(std::string(s));
                s = dataptr + i + 1;
                j = 0;
            }
        }
        printf("deserializing string vector [ \033[1;32mor matrix\033[0m ] siz %ld obtained vector of %ld strings\n", datasiz, vs.size());
        for(size_t i = 0; i < vs.size(); i++)
            printf("'%s'\n", vs[i].c_str());

        return f == CuVariant::Vector ? CuVariant(vs) : CuVariant(vs, rows, vs.size() / rows);
    }
    return CuVariant();
}

// put the rows of a matrix of strings one after another
char **CuDataSerializer::std_string_matrix_to_char_p(const CuVariant &v) const {
    size_t siz, i = 0;
    char **p = new char *[v.getSize()]; // v.getSize = rows x cols
    CuMatrix<std::string> m = v.toMatrix<std::string>();
    for(size_t r = 0; r < m.nrows(); r++) {
        for(size_t c = 0; c < m.ncols(); c++) {
            siz = m[r][c].length() + 1;
            p[i] = new char[siz];
            memset(p[i], 0, sizeof(siz));
            strncpy(p[i], m[r][c].c_str(), siz);
            i++;
        }
    }
    return p;
}
