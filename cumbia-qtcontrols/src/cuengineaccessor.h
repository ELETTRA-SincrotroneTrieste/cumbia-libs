#ifndef CUENGINEACCESSOR_H
#define CUENGINEACCESSOR_H

#include "cucontrolsfactorypool.h"
#include <QObject>

class CumbiaPool;
class CuControlsFactoryPool;

class CuEngineAccessorP;

class CuEngineAccessor : public QObject
{
    Q_OBJECT
public:
    explicit CuEngineAccessor(QObject *parent, CumbiaPool **cu_p_p, CuControlsFactoryPool *fp_p);
    virtual ~CuEngineAccessor();

    virtual void engine_swap(CumbiaPool *cu_p, const CuControlsFactoryPool &fp);
    CumbiaPool *cu_pool() const;
    CuControlsFactoryPool *f_pool() const;

signals:
    void engineSwapped();
    void engineChanged(const QStringList &from, const QStringList& to);

private:
    CuEngineAccessorP *d;

};

#endif // CUENGINEACCESSOR_H
