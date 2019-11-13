#ifndef CUTHREADTOKENGENI_H
#define CUTHREADTOKENGENI_H

#include <cudata.h>

class CuThreadTokenGenI
{
public:
    virtual ~CuThreadTokenGenI() {}

    virtual CuData generate(const CuData& in) = 0;
};

#endif // CUTHREADTOKENGENI_H
