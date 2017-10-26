#include "$HFILE$"
#include "ui_$HFILE$"

#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cumacros.h>

$MAINCLASS$::$MAINCLASS$(CumbiaEpics *cue, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::$MAINCLASS$)
{
    cu_e = cue;
    m_log = new CuLog(&m_log_impl);
    cu_e->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui->setupUi(this, cu_e, cu_ep_r_fac, cu_ep_w_fac);
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
