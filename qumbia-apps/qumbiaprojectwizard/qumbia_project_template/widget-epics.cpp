#include "$HFILE$"
#include "ui_$HFILE$"

#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>

$MAINCLASS$::$MAINCLASS$(CumbiaEpics *cue, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::$UIFORMCLASS$)
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
