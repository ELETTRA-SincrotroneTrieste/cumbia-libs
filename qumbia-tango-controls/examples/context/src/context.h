#ifndef Options_H
#define Options_H

#include <QWidget>
#include <cutreader.h>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

class CumbiaPool;

namespace Ui {
class Options;
}

class CumbiaTango;

class Context : public QWidget
{
    Q_OBJECT

public:
    explicit Context(CumbiaPool *cumbia_pool, QWidget *parent = 0);
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
    CumbiaPool *cu_pool;
    CuControlsFactoryPool m_ctrl_factory_pool;
    QuLogImpl m_log_impl;
    CuLog *m_log;

    CuTReader::RefreshMode m_getRefreshMode();
};

#endif // Options_H
