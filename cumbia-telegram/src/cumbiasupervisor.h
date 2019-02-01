#ifndef CUMBIASUPERVISOR_H
#define CUMBIASUPERVISOR_H

class CumbiaPool;

#include <cucontrolsfactorypool.h>

class CumbiaSupervisor
{
public:
    CumbiaSupervisor();

    void setup();

    void dispose();


    // cumbia stuff
    CuControlsFactoryPool ctrl_factory_pool;
    CumbiaPool *cu_pool;

};

#endif // CUMBIASUPERVISOR_H
