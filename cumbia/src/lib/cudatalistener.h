#ifndef CUTANGOLISTENER_H
#define CUTANGOLISTENER_H

class CuData;
class CuTangoActionI;

#include <stdio.h>

class CuDataListenerPrivate;

class CuDataListener
{
public:

    CuDataListener();

    virtual ~CuDataListener();
 
    virtual void onUpdate(const CuData& data) = 0;

    virtual bool invalid() const;

    virtual bool invalidate();

    virtual void setValid();

private:
    CuDataListenerPrivate *d;
};

#endif // CUTANGOLISTENER_H
