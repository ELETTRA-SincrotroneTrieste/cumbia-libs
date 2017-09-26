#include "culinkcontrol.h"

#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>

CuLinkControl::CuLinkControl(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
{
    cu = cumbia;
    r_factory = r_fac.clone();
    w_factory = NULL;
    cu_pool = NULL;

}

CuLinkControl::CuLinkControl(Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
{
    cu = cumbia;
    w_factory = w_fac.clone();
    r_factory = NULL;
    cu_pool = NULL;
}

CuLinkControl::CuLinkControl(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
{
    ctrl_factory_pool = fpool;
    cu_pool = cumbia_pool;
    cu = NULL;
    r_factory = NULL;
    w_factory = NULL;
}

CuLinkControl::~CuLinkControl()
{
    if(r_factory)
        delete r_factory;
    if(w_factory)
        delete w_factory;
}

bool CuLinkControl::isAuto() const
{
    return cu_pool != NULL;
}

CuControlsReaderA *CuLinkControl::make_reader(const std::string &s, CuDataListener* datal) const
{
    CuControlsReaderA *reader = NULL;
    Cumbia *cumbia = NULL;
    CuControlsReaderFactoryI *r_fa;
    if(cu_pool && !ctrl_factory_pool.isEmpty()) /* pick Cumbia impl */
    {
        // pick a cumbia and reader factory implementation from the pool
        cumbia = cu_pool->getBySrc(s);
        r_fa = ctrl_factory_pool.getRFactoryBySrc(s);
    }
    else
    {
        // use specific cumbia and reader factory implementations
        cumbia = cu;
        r_fa = r_factory;
    }

    if(!cumbia || !r_fa)
        return reader;

    reader = r_fa->create(cumbia, datal);

    return reader;
}
