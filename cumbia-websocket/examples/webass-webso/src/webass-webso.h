#ifndef WebassWebso_H
#define WebassWebso_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

class CuData;
class CumbiaPool;

class WebassWebso : public QWidget
{
    Q_OBJECT

public:
    explicit WebassWebso(CumbiaPool *cu_p, QWidget *parent = 0);
    ~WebassWebso();

private:

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // WebassWebso_H
