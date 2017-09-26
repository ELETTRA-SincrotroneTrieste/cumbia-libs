#ifndef CUSERVICE_I_H
#define CUSERVICE_I_H

#include <cuservices.h>
#include <string>

class CuServiceI
{
public:
    virtual ~CuServiceI() {}

    virtual std::string getName() const = 0;

    virtual CuServices::Type getType() const = 0;
};

#endif // CUSERVICEI_H
