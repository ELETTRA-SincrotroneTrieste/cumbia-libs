#ifndef Cumbiaplots_H
#define Cumbiaplots_H

#include <QWidget>

// cumbia-tango
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>
// cumbia-tango

namespace Ui {
class Cumbiaplots;
}

class Cumbiaplots : public QWidget
{
    Q_OBJECT

public:
    explicit Cumbiaplots(CumbiaTango *cut, QWidget *parent = 0);
    ~Cumbiaplots();

private slots:
    void updateAll();

private:
    Ui::Cumbiaplots *ui;

    int m_nPoints;

// cumbia-tango
    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
// cumbia-tango
};

#endif // Cumbiaplots_H
