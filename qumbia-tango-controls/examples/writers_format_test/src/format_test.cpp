#include "format_test.h"
#include "ui_format_test.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia

Format_test::Format_test(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::Format_test;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // mloader.modules() to get the list of loaded modules
    // cumbia
}

Format_test::~Format_test()
{
    delete ui;
}
