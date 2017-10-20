#include "cudatalistener.h"


class CuDataListenerPrivate
{
public:
    bool invalid;
};

CuDataListener::CuDataListener() {
    d = new CuDataListenerPrivate;
    d->invalid = false;
}

bool CuDataListener::invalidate()
{
    d->invalid = true;
}

void CuDataListener::setValid()
{
    d->invalid = false;
}

CuDataListener::~CuDataListener() {
    delete d;
}

bool CuDataListener::invalid() const
{
    return d->invalid;
}

