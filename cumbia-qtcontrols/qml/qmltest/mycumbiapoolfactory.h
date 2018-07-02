#ifndef MYCUMBIAPOOLFACTORY_H
#define MYCUMBIAPOOLFACTORY_H

class CumbiaPool;
#include <QObject>
#include <cucontrolsfactorypool.h>

class CumbiaPoolFactory : public QObject
{
    Q_OBJECT
public:
    CumbiaPoolFactory(QObject *parent = nullptr);

    void init(CumbiaPool *cupoo, const CuControlsFactoryPool &fapool)
    {
        m_cu_pool = cupoo;
        m_fac = fapool;
    }

    virtual CumbiaPool *getPool() const
    {
        printf("CumbiaPoolFactory.getPool returning %p\n", m_cu_pool);
        return m_cu_pool;
    }

    CuControlsFactoryPool getFactory() const
    {
        return m_fac;
    }

signals:

public slots:

private:
    CumbiaPool *m_cu_pool;

    CuControlsFactoryPool m_fac;
};

#endif // MYCUMBIAPOOLFACTORY_H


