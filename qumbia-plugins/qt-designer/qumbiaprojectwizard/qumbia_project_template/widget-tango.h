#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QWidget>
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>

class CumbiaTango;

namespace Ui {
class $MAINCLASS$;
}

class CumbiaTango;

class $MAINCLASS$ : public QWidget
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(CumbiaTango *cut, QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$MAINCLASS$ *ui;

    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
};

#endif // $MAINCLASS$_H
