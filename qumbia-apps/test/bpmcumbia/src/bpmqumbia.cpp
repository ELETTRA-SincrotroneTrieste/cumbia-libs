#include "bpmqumbia.h"
// cumbia-tango
#include <cuserviceprovider.h>
#include <cucontextactionbridge.h>
#include <cumacros.h>
// cumbia-tango

#include <quspectrumplot.h>
#include <qwt_plot_canvas.h>
#include "ui_bpmqumbia.h"

Bpmqtango::Bpmqtango(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)	/* ,    ui(new Ui::Bpmqtango)	  // ## qumbiaprojectwizard: instantiated in constructor body */
{

// cumbia-tango
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui = new Ui::bpmqumbia;
    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    // cumbia-tango


    //ui->setupUi(this);

    int period = 100;
    bool ok;
    foreach (QString arg, qApp->arguments()) {
        if(arg.toInt(&ok) && ok)
            period = arg.toInt();
    }


    printf("options \"--cmds\" to create three plots with commands\n");
    printf("        \"--attrs\" to create one plot with three attributes");


    if(qApp->arguments().contains("--cmds")) {
        QuSpectrumPlot *sp = new QuSpectrumPlot(this, cut, cu_tango_r_fac);
        sp->setSource("$1->GetHorPos(0,1000)");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp);
        QuSpectrumPlot *sp_ver = new QuSpectrumPlot(this, cut, cu_tango_r_fac);
        sp_ver->setSource("$1->GetVerPos(0,1000)");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp_ver);
        QuSpectrumPlot *sp_sum = new QuSpectrumPlot(this, cut, cu_tango_r_fac);
        sp_sum->setSource("$1->GetSum(0,1000)");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp_sum);
    }

    if(qApp->arguments().contains("--attrs")) {
        QuSpectrumPlot *sp = new QuSpectrumPlot(this, cut, cu_tango_r_fac);
        sp->setSource("$1/VaPickup;$1/VbPickup;$1/VcPickup;$1/VdPickup");
//        sp->setSource("$1/VaPickup;$1/VbPickup");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp);
    }

//     foreach(TPlotLightMarker *plot, findChildren<TPlotLightMarker *>())
    foreach(QuSpectrumPlot *plot, findChildren<QuSpectrumPlot *>()) {//	TPlotLightMarker is mapped into QuTrendPlot OR QuSpectrumPlot. Please check.	//	 ## added by qumbiaprojectwizard
        plot->setPeriod(period);
        printf("enabling autoscale\n");
        plot->setYAxisAutoscaleEnabled(true);
        printf("autoscale enabled? FDAFYGA %d\n", plot->yAxisAutoscaleEnabled());
//       plot->setRefreshTimeout(100);

        QwtPlotCanvas *c = qobject_cast<QwtPlotCanvas *>(plot->canvas());
        printf("plot canvas paint backing store %d opaque %d HackStyledBackground  %d ImmediatePaint %d\n",
               c->testPaintAttribute(QwtPlotCanvas::BackingStore),
               c->testPaintAttribute(QwtPlotCanvas::Opaque),
              c->testPaintAttribute(QwtPlotCanvas::HackStyledBackground),
               c->testPaintAttribute(QwtPlotCanvas::ImmediatePaint));


    }
    new CuContextActionBridge(this, cu_t, cu_tango_r_fac);

}

Bpmqtango::~Bpmqtango()
{
    delete ui;
}

