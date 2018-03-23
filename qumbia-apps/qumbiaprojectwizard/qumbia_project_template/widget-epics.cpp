#include "$HFILE$"
#include "ui_$HFILE$"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cucontextactionbridge.h>
#include <cumacros.h>
#include <cumbiaepics.h>

$MAINCLASS$::$MAINCLASS$(CumbiaEpics *cue, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::$UIFORMCLASS$)
{
    cu_e = cue;
    m_log = new CuLog(&m_log_impl);
    cu_e->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui->setupUi(this, cu_e, cu_ep_r_fac, cu_ep_w_fac);

    // after all cumbia widgets have been instantiated
    // if you add widgets later, you can use the connectObject(QObject *w)
    // method from CuContextActionBridge.
    new CuContextActionBridge(this, cu_e, cu_ep_r_fac);
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
