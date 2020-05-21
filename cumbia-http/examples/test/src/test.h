#ifndef Test_H
#define Test_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

namespace Ui {
class Test;
}

class CuData;
class CumbiaPool;

class Test : public QWidget
{
    Q_OBJECT

public:
    explicit Test(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Test();

private:
    Ui::Test *ui;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuLog *m_log;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // Test_H
