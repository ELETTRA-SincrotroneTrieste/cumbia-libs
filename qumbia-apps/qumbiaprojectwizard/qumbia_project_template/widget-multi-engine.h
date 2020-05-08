#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

namespace Ui {
class $UIFORMCLASS$;
}

class CuData;
class CumbiaPool;

class $MAINCLASS$ : public QWidget
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(CumbiaPool *cu_p, QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$UIFORMCLASS$ *ui;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // $MAINCLASS$_H
