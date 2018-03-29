#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QWidget>

// cumbia-tango
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>
// cumbia-tango

namespace Ui {
class $UIFORMCLASS$;
}

class $MAINCLASS$ : public QWidget
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(CumbiaTango *cut, QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$UIFORMCLASS$ *ui;

// cumbia-tango
    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
// cumbia-tango
};

#endif // $MAINCLASS$_H
