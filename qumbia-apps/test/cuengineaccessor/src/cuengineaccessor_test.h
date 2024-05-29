#ifndef Cuengineaccessor_H
#define Cuengineaccessor_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Cuengineaccessor;
}

class CuengineaccessorTest : public QWidget
{
    Q_OBJECT

public:
    explicit CuengineaccessorTest(QWidget *parent = 0);
    ~CuengineaccessorTest();

private:
    Ui::Cuengineaccessor *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // Cuengineaccessor_H
