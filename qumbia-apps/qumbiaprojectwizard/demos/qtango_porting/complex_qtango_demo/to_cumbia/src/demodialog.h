#ifndef DEMODIALOG_H
#define DEMODIALOG_H

// cumbia-tango
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>
// cumbia-tango

#include <QDialog>

namespace Ui {
class DemoDialog;
}

class DemoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DemoDialog(CumbiaTango *cut, QWidget *parent = 0);
    ~DemoDialog();

private:
    Ui::DemoDialog *ui;

    // cumbia-tango
        CumbiaTango *cu_t;
        CuTReaderFactory cu_tango_r_fac;
        CuTWriterFactory cu_tango_w_fac;
        QuLogImpl m_log_impl;
        CuLog *m_log;
    // cumbia-tango
};

#endif // DEMODIALOG_H

