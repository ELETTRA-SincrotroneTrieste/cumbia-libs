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
    connect(ui->pbStop, SIGNAL(clicked()), this, SLOT(onStopSrc()));
}

HttpTest::~HttpTest()
{
    delete ui;
}

void HttpTest::onStopSrc() {
    QList<QCheckBox *> cbs = findChildren<QCheckBox *>(QRegularExpression("cb.*"));
    QList<QWidget *> wids;
    foreach(QCheckBox *cb, cbs) {
        if(cb->isChecked()) wids << findChild<QWidget *>(cb->objectName().remove("cb"));
    }
    foreach(QWidget *w, wids) {
        if(qobject_cast<QuLabel *>(w)) qobject_cast<QuLabel *>(w)->unsetSource();
        else if(qobject_cast<QuTrendPlot *>(w)) qobject_cast<QuTrendPlot *>(w)->unsetSources();
        else if(qobject_cast<QuCircularGauge *>(w)) qobject_cast<QuCircularGauge *>(w)->unsetSource();
    }
}
