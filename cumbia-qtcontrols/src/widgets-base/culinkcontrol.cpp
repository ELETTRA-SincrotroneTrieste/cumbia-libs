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

CuControlsWriterA *CuLinkControl::make_writer(const std::string &s, CuDataListener *datal) const
{
    CuControlsWriterA *writer = NULL;
    Cumbia *cumbia = NULL;
    CuControlsWriterFactoryI *w_fa;
    printf("\e[1;35mCuLinkControl.make_writer: src %s cu_pool %p ctrl_factroypool isEmpty %d\e[0m", s.c_str(), ctrl_factory_pool,
           ctrl_factory_pool.isEmpty());
    if(cu_pool && !ctrl_factory_pool.isEmpty()) /* pick Cumbia impl */
    {
        // pick a cumbia and reader factory implementation from the pool
        cumbia = cu_pool->getBySrc(s);
        w_fa = ctrl_factory_pool.getWFactoryBySrc(s);
    }
    else
    {
        // use specific cumbia and reader factory implementations
        cumbia = cu;
        w_fa = w_factory;
    }

    if(!cumbia || !w_fa)
        return writer;

    writer = w_fa->create(cumbia, datal);

    printf("\e[1;35mCuLinkControl.make_writer: got cumbia %p and w_factort %p writer is %p\e[0m\n", cumbia, w_fa, writer);
    return writer;
}
