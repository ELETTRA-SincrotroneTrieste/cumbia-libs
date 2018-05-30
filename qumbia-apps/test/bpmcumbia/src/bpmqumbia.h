#ifndef Bpmqtango_H
#define Bpmqtango_H

// cumbia-tango
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>
// cumbia-tango

#include <QWidget>

namespace Ui {
class bpmqumbia;
}

class Bpmqtango : public QWidget
{
    Q_OBJECT

public:
    explicit Bpmqtango(CumbiaTango *cut, QWidget *parent = 0);
    ~Bpmqtango();

private:
    Ui::bpmqumbia *ui;
// cumbia-tango
    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
// cumbia-tango

};

#endif // Bpmqtango_H

