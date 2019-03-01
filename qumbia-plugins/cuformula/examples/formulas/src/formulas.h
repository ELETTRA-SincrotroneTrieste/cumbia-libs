#ifndef Testtest_H
#define Testtest_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>

namespace Ui {
class Testtest;
}

class CuData;
class CumbiaPool;

class Formulas : public QWidget
{
    Q_OBJECT

public:
    explicit Formulas(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Formulas();

private slots:

private:
    Ui::Testtest *ui;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // Testtest_H
