#ifndef QUIOWIDGET_H
#define QUIOWIDGET_H

#include <QWidget>
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>

class CumbiaTango;

namespace Ui {
class Quinputoutput;
}

class CumbiaTango;

class Quinputoutput : public QWidget
{
    Q_OBJECT

public:
    explicit Quinputoutput(CumbiaTango *cut, QWidget *parent = 0);
    ~Quinputoutput();

private:
    Ui::Quinputoutput *ui;

    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
};

#endif // Quinputoutput_H
