#include "engine-switch.h"
#include "ui_engine-switch.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia

Engineswitch::Engineswitch(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::Engineswitch;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // mloader.modules() to get the list of loaded modules
    // cumbia
}

Engineswitch::~Engineswitch()
{
    delete ui;
}
