#ifndef CUTANGOLISTENER_H
#define CUTANGOLISTENER_H

class CuData;
class CuTangoActionI;

#include <stdio.h>

class CuDataListener
{
public:
    virtual ~CuDataListener() { }
 
    virtual void onUpdate(const CuData& d) = 0;
};

#endif // CUTANGOLISTENER_H
