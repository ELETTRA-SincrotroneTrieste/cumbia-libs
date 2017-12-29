#ifndef Multireader_H
#define Multireader_H

#include <QWidget>
#include <qulogimpl.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>

class CumbiaTango;

namespace Ui {
class Multireader;
}

class CumbiaTango;
class QuMultiReaderPluginInterface;

class Multireader : public QWidget
{
    Q_OBJECT

public:
    explicit Multireader(CumbiaTango *cut, QWidget *parent = 0);
    ~Multireader();

private slots:
    void newData(const CuData&);
    void seqReadComplete(const QList<CuData >&);

private:
    Ui::Multireader *ui;

    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;

    void m_loadMultiReaderPlugin();

    QuMultiReaderPluginInterface* m_multir;
    bool m_sequential;
};

#endif // Multireader_H
