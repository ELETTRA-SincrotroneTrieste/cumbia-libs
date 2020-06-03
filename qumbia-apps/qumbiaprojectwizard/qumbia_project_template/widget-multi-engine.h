#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

namespace Ui {
class $UIFORMCLASS$;
}

class $MAINCLASS$ : public QWidget
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(CumbiaPool *cu_p, QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$UIFORMCLASS$ *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia
};

#endif // $MAINCLASS$_H
