#ifndef Manual_refresh_H
#define Manual_refresh_H

#include <QWidget>

// cumbia-tango
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>
// cumbia-tango

class Manual_refresh : public QWidget
{
    Q_OBJECT

public:
    explicit Manual_refresh(CumbiaPool *cup, QWidget *parent = 0);
    ~Manual_refresh();

private slots:
    void read();

private:

// cumbia-tango
    CumbiaPool *cu_p;
    CuControlsFactoryPool m_ctrl_factory_pool;
    QuLogImpl m_log_impl;
    CuLog *m_log;
// cumbia-tango
};

#endif // Manual_refresh_H
