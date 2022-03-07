#ifndef Cu_qtcontrols_widgets_H
#define Cu_qtcontrols_widgets_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

class CuData;
class CumbiaPool;

class Cu_qtcontrols_widgets : public QWidget
{
    Q_OBJECT

public:
    explicit Cu_qtcontrols_widgets(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Cu_qtcontrols_widgets();

public slots:
    void onTimeo();

private:

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;

    int m_cnt;
};

#endif // Cu_qtcontrols_widgets_H
