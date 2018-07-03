#include "cumbiapool_o.h"

CumbiaPool_O::CumbiaPool_O(QObject *parent) : QObject(parent)
{
    m_cu_pool = NULL;
}

void CumbiaPool_O::init(CumbiaPool *cupoo, const CuControlsFactoryPool &fapool)
{
    m_cu_pool = cupoo;
    m_fac = fapool;
}

CuControlsFactoryPool CumbiaPool_O::getFactory() const
{
    return m_fac;
}

CumbiaPool *CumbiaPool_O::getPool() const
{
    printf("retunring cumbia pool %p\n", m_cu_pool);
    return m_cu_pool;
}
