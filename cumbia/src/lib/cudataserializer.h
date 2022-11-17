#ifndef CUDATASERIALIZER_H
#define CUDATASERIALIZER_H

#include <cudata.h>
#include <list>

#define  VERSION 0x0002
#define CUDATASER_MAXSRCLEN 128 // 32 bytes aligned!

// #pragma pack(1)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


struct repr {
    uint32_t size;

    uint8_t version;
    uint8_t type;
    uint8_t format;
    uint8_t stat; // 0|0|0|0|0|e|x|y  e: 0 LE, 1 BE, x = 1: event x = 0: poll y = 0 ok y = 1 ERR

    char src[CUDATASER_MAXSRCLEN];

    uint64_t timestamp;
    uint32_t datasiz;
    uint32_t rows;
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
    int rows = datasiz + sizeof(uint32_t);
    int msgsiz = rows + sizeof(uint32_t);

    int totlen = msgsiz + sizeof(uint32_t);
};

static struct repr_offsets roffsets;

/*! \brief serialize CuData to send it over the network or save it into a file
 */
class CuDataSerializer
{
public:
    char* serialize(const CuData& da) const;

    uint32_t size(const char *data) const;
    uint32_t data_size(const char *data) const;
    uint32_t rows(const char *data) const;
    uint8_t version(const char *data) const;

    const char *data_ptr(const char *data) const;
    const char *src(const char *data) const;
    const char *msg_ptr(const char *data) const;
    std::string message(const char *data) const;
    uint32_t msg_len(const char *data) const;

    CuData deserialize(const char *data, size_t len) const;

private:
    template <typename T> char *data_init_t(struct repr *re, size_t datalen, size_t msglen, size_t rows = 1) const;
    char *data_s_init(struct repr *re, char **p, size_t len, size_t msglen, size_t nrows = 1) const;
    template <typename T> char* serialize_t(char *buf, struct repr *re, T *p) const;
    char *serialize_string(char *buf, repr *re, char **p, size_t len) const;
    template <typename T> CuVariant deserialize_data_t(const char *data, size_t siz, CuVariant::DataType t, CuVariant::DataFormat f, size_t rows = 1) const;
    CuVariant deserialize_string(const char *buf, size_t siz, CuVariant::DataFormat f, size_t rows) const;
    char **std_string_matrix_to_char_p(const CuVariant& v) const;
};


#endif // CUDATASERIALIZER_H
