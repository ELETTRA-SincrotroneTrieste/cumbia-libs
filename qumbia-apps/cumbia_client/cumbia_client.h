#ifndef CUGENERIC_CLIENT_H
#define CUGENERIC_CLIENT_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

namespace Ui {
class CumbiaClient;
}

class CuData;
class CumbiaPool;

class CumbiaClient : public QWidget
{
    Q_OBJECT

public:
    explicit CumbiaClient(CumbiaPool *cupool, QWidget *parent = 0);
    ~CumbiaClient();

private slots:
    void configure(const CuData& d);

    void changeRefresh();

    void sourcesChanged();

    void unsetSources();

    void switchSources();

private:
    Ui::CumbiaClient *ui;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuLog *m_log;
    CuControlsFactoryPool m_ctrl_factory_pool;
    QStringList m_oldSrcs;

    int m_switchCnt;

    bool m_do_conf;

    const int m_layoutColumnCount;
};

#endif // CUTCWIDGETS_H
