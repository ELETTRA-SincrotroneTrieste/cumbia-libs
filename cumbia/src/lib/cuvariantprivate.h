#ifndef CUVARIANTPRIVATE_H
#define CUVARIANTPRIVATE_H

#include <stdlib.h>

class CuVariantPrivate
{
public:
    CuVariantPrivate();

    size_t mSize;

    int format; // DataFormat

    int type; // DataType

    bool mIsValid;

    bool mIsNull;

    void * val;

};

#endif // XVARIANTPRIVATE_H
