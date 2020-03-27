#ifndef Randomwi_H
#define Randomwi_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

namespace Ui {
class Randomwi;
}

class CuData;
class CumbiaPool;

class Randomwi : public QWidget
{
    Q_OBJECT

public:
    explicit Randomwi(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Randomwi();

private:
    Ui::Randomwi *ui;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // Randomwi_H
