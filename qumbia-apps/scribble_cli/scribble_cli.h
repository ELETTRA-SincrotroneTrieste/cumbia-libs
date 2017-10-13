#ifndef CUGENERIC_CLIENT_H
#define CUGENERIC_CLIENT_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

namespace Ui {
class GenericClient;
}

class CuData;
class CumbiaPool;

class GenericClient : public QWidget
{
    Q_OBJECT

public:
    explicit GenericClient(CumbiaPool *cupool, QWidget *parent = 0);
    ~GenericClient();

private slots:
    void configure(const CuData& d);

    void changeRefresh();

    void sourcesChanged();

    void unsetSources();

    void switchSources();

private:
    Ui::GenericClient *ui;

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
