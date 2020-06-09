#include "httptest.h"
#include "ui_httptest.h"

#include <quapps.h>

HttpTest::HttpTest(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    cu_pool(cumbia_pool),
    ui(new Ui::HttpTest)
{
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    // needs DEFINES -= QT_NO_DEBUG_OUTPUT in .pro
    qDebug() << __PRETTY_FUNCTION__ << "available engines" << mloader.modules();
    ui->setupUi(this, cumbia_pool, m_ctrl_factory_pool);
}

HttpTest::~HttpTest()
{
    delete ui;
}
