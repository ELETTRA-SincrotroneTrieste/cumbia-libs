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
    explicit $MAINCLASS$(QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$UIFORMCLASS$ *ui;
    QuLogImpl m_log_impl;
};

#endif // $MAINCLASS$_H
