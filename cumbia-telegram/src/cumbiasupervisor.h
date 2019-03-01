#ifndef CUMBIASUPERVISOR_H
#define CUMBIASUPERVISOR_H

class CumbiaPool;
class CuFormulaPluginI;

#include <cucontrolsfactorypool.h>


class CumbiaSupervisor
{
public:
    CumbiaSupervisor();

    void setup();

    void dispose();

    CuFormulaPluginI *formulaPlugin() const;


    // cumbia stuff
    CuControlsFactoryPool ctrl_factory_pool;
    CumbiaPool *cu_pool;

private:
    CuFormulaPluginI* m_formulaPlu;

};

#endif // CUMBIASUPERVISOR_H
