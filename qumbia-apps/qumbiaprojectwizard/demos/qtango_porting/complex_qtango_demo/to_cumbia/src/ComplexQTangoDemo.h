#ifndef ComplexQTangoDemo_H
#define ComplexQTangoDemo_H

// cumbia-tango
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>
// cumbia-tango

#include <QWidget>

namespace Ui {
class ComplexQTangoDemo;
}

class ComplexQTangoDemo : public QWidget
{
    Q_OBJECT

public:
    explicit ComplexQTangoDemo(CumbiaTango *cut, QWidget *parent = 0);
    ~ComplexQTangoDemo();

private slots:
    void execDemoDialog();

private:
//     void m_setup(); // install QTWatcher and QTWriter, set source/targets on custom reader and writer
    void m_setup(); // install QuWatcher and QTWriter, set source/targets on custom reader and writer//	QTWatcher is mapped into QuWatcher	//	 ## added by qumbiaprojectwizard
    Ui::ComplexQTangoDemo *ui;
// cumbia-tango
    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;
// cumbia-tango

};

#endif // ComplexQTangoDemo_H

