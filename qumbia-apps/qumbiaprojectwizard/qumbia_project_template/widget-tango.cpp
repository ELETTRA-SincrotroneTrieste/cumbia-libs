#include "$HFILE$"
#include "ui_$HFILE$"

// cumbia-tango
#include <cuserviceprovider.h>
#include <cumacros.h>
// cumbia-tango

$MAINCLASS$::$MAINCLASS$(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)
{
    // cumbia-tango
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui = new Ui::$UIFORMCLASS$;
    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    // cumbia-tango
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
