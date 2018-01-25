#ifndef Options_H
#define Options_H

#include <QWidget>
#include <qulogimpl.h>
#include <cutreader.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumbiatango.h>

class CumbiaTango;

namespace Ui {
class Options;
}

class CumbiaTango;

class Context : public QWidget
{
    Q_OBJECT

public:
    explicit Context(CumbiaTango *cut, QWidget *parent = 0);
    ~Context();

public slots:
    void setMode();

    void setPeriod();

    void reloadProps();

private slots:

    void runModified();

    void onNewData(const CuData& d);

    void getData();

    void sendData();

    void onLinkStatsRequest(QWidget* w);

private:
    Ui::Options *ui;

    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
    QuLogImpl m_log_impl;
    CuLog *m_log;

    CuTReader::RefreshMode m_getRefreshMode();
};

#endif // Options_H
