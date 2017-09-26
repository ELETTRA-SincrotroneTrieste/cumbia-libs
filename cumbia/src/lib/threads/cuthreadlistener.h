#ifndef CUTHREADLISTENER_H
#define CUTHREADLISTENER_H

#include <cudata.h>

class CuThreadListener
{
public:
    CuThreadListener();

    virtual ~CuThreadListener() { }

    virtual void onProgress(int step, int total, const CuData& data) = 0;

    virtual void onResult(const CuData& data) = 0;

    virtual CuData getToken() const = 0;
};

#endif // CUTHREADLISTENER_H
