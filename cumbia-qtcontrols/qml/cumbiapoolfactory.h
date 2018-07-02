#ifndef CUMBIAPOOLFACTORY_H
#define CUMBIAPOOLFACTORY_H

#include <QObject>
#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>

class CumbiaPoolFactory : public QObject
{
    Q_OBJECT
public:
    explicit CumbiaPoolFactory(QObject *parent = nullptr) : QObject(parent)
    {
        m_cu_pool = NULL;
    }

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

#endif // CUMBIAPOOLFACTORY_H
