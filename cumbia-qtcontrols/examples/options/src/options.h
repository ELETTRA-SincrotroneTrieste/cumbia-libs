#ifndef Options_H
#define Options_H

#include <QWidget>
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>

class CumbiaTango;

namespace Ui {
class Options;
}

class CumbiaTango;

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(CumbiaTango *cut, QWidget *parent = 0);
    ~Options();

private:
    Ui::Options *ui;

    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
};

#endif // Options_H
