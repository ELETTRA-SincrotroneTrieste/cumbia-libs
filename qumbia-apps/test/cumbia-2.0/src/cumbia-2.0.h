#ifndef Cumbia2_H
#define Cumbia2_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Cumbia2;
}

class Cumbia2 : public QObject
{
    Q_OBJECT

public:
    explicit Cumbia2(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Cumbia2();

private:

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // Cumbia2_H
