#ifndef Editargs_H
#define Editargs_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class Editargs;
}

class Editargs : public QWidget
{
    Q_OBJECT

public:
    explicit Editargs(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Editargs();

private slots:
    void onSbValueChanged(double v);
    void onReadClicked();
private:
    Ui::Editargs *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // Editargs_H
