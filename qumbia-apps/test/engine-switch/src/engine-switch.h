#ifndef Engineswitch_H
#define Engineswitch_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Engineswitch;
}

class Engineswitch : public QWidget
{
    Q_OBJECT

public:
    explicit Engineswitch(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Engineswitch();

private:
    Ui::Engineswitch *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // Engineswitch_H
