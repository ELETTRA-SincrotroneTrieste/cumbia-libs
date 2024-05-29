#include "cuengineaccessor_test.h"
#include "ui_cuengineaccessor_test.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia

CuengineaccessorTest::CuengineaccessorTest(QWidget *parent) :
    QWidget(parent) {
    cu_pool = new CumbiaPool();
    new CuEngineAccessor(this, &cu_pool, &m_ctrl_factory_pool);
    CuModuleLoader mloader(cu_pool, &m_ctrl_factory_pool, &m_log_impl);
    ui = new Ui::Cuengineaccessor;
    ui->setupUi(this);
}

CuengineaccessorTest::~CuengineaccessorTest()
{
    printf("deleting UI...\n");
    delete ui;
    printf("done deleting UI\n\n");

}
