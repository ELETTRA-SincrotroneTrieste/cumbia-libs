#ifndef Manual_refresh_H
#define Manual_refresh_H

#include <QWidget>

// cumbia-tango
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>
// cumbia-tango

class Manual_refresh : public QWidget
{
    Q_OBJECT

public:
    explicit Manual_refresh(CumbiaTango *cut, QWidget *parent = 0);
    ~Manual_refresh();

private slots:
    void read();

private:

// cumbia-tango
    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
// cumbia-tango
};

#endif // Manual_refresh_H
