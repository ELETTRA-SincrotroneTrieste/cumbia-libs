#include "httptest.h"
#include "ui_httptest.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#include <quapps.h>

HttpTest::HttpTest(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HttpTest)
{
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // needs DEFINES -= QT_NO_DEBUG_OUTPUT in .pro
    qDebug() << __PRETTY_FUNCTION__ << "available engines" << mloader.modules();

}

HttpTest::~HttpTest()
{
    delete ui;
}
