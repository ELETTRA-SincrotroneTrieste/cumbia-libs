#ifndef CUDATASERIALIZER_H
#define CUDATASERIALIZER_H

#include <cudata.h>
#include <list>

#define  VERSION 0x0001
#define CUDATASER_MAXSRCLEN 128 // 32 bytes aligned!

// #pragma pack(1)

struct repr {
    uint32_t size;

    uint8_t version;
    uint8_t type;
    uint8_t format;
    uint8_t stat; // 0|0|0|0|0|e|x|y  e: 0 LE, 1 BE, x = 1: event x = 0: poll y = 0 ok y = 1 ERR

    char src[CUDATASER_MAXSRCLEN];

    uint64_t timestamp;
    uint32_t datasiz;
    uint32_t msgsiz;
};

struct repr_offsets {
    int size = 0;
    int version = size + sizeof(uint32_t);
    int type = version + sizeof(uint8_t);
    int format = type + sizeof(uint8_t);
    int stat = format + sizeof(uint8_t);

    int src = stat + sizeof(uint8_t);

    int timestamp = src + sizeof(char) * CUDATASER_MAXSRCLEN;

    int datasiz = timestamp + sizeof(uint64_t);
    int msgsiz = datasiz + sizeof(uint32_t);

    int totlen = msgsiz + sizeof(uint32_t);
} ;

static struct repr_offsets roffsets;


class CuDataSerializer
{
public:
    char* serialize(const CuData& da) const;

    uint32_t size(const char *data) const;
    uint32_t data_size(const char *data) const;
    uint8_t version(const char *data) const;

    const char *data_ptr(const char *data) const;
    const char *src(const char *data) const;

    CuData deserialize(const char *data) const;

private:
    template <typename T> char* serialize_t(struct repr *re, T *p, size_t len) const;
    char *serialize_string(repr *re, char **p, size_t len) const;
    template <typename T> CuVariant deserialize_data_t(const char *data, size_t siz, CuVariant::DataType t, CuVariant::DataFormat f) const;
    CuVariant deserialize_string(const char *buf, size_t siz, CuVariant::DataFormat f) const;
};


#endif // CUDATASERIALIZER_H
