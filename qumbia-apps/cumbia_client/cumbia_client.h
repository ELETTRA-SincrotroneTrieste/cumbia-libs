#ifndef CUGENERIC_CLIENT_H
#define CUGENERIC_CLIENT_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>

/*! \mainpage
 *
 * *cumbia_client* is a generic *engine-independent* client to read/write from Tango, Epics, ... control systems
 *
 * At the moment of writing this document, the available environments are
 *
 * - <a href="http://www.tango-controls.org">Tango</a>
 * - <a href="https://epics.anl.gov/">Epics</a>
 *
 * The two engines are implemented in the cumbia-tango and cumbia-epics modules.
 *
 * \par Example usage
 *
 * \code
 * cumbia client test/device/1/double_scalar   test/device/2/long_scalar  giacomo:ai1
 * \endcode
 *
 * where cumbia client is a shortcut for cumbia_client.
 * Two *tango attributes* and one *epics analog variable* follow.
 *
 */

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
