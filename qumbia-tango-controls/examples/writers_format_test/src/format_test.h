#ifndef Format_test_H
#define Format_test_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Format_test;
}

class Format_test : public QWidget
{
    Q_OBJECT

public:
    explicit Format_test(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Format_test();

private:
    Ui::Format_test *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // Format_test_H
