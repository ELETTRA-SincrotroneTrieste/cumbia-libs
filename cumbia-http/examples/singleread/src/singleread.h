#ifndef Singleread_H
#define Singleread_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Singleread;
}

class Singleread : public QWidget
{
    Q_OBJECT

public:
    explicit Singleread(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Singleread();

private:
    Ui::Singleread *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // Singleread_H
