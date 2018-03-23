#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QWidget>
#include <qulogimpl.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cumbiaepics.h>

class CumbiaTango;

namespace Ui {
class $UIFORMCLASS$;
}

class CumbiaTango;

class $MAINCLASS$ : public QWidget
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(CumbiaEpics *cuep, QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$UIFORMCLASS$ *ui;

    CumbiaEpics *cu_e;
    CuEpReaderFactory cu_ep_r_fac;
    CuEpWriterFactory cu_ep_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
};

#endif // $MAINCLASS$_H
