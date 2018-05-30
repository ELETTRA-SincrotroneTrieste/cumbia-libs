#include "bpmqtango.h"
#include "ui_bpmqtango.h"
#include <tutil.h>
#include <TPlotLightMarker>

Bpmqtango::Bpmqtango(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Bpmqtango)
{
    ui->setupUi(this);

   TUtil::instance()->setGlobalRefreshDisabled(qApp->arguments().contains("--global-refresh-disabled"));

    int period = 100;
    bool ok;
    foreach (QString arg, qApp->arguments()) {
        if(arg.toInt(&ok) && ok)
            period = arg.toInt();
    }

    printf("options \"--cmds\" to create three plots with commands\n");
    printf("        \"--attrs\" to create one plot with three attributes\n");

    if(qApp->arguments().contains("--cmds")) {
        TPlotLightMarker *sp = new TPlotLightMarker(this);
        sp->setSources("$1->GetHorPos(0,1000)");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp);
        TPlotLightMarker *sp_ver = new TPlotLightMarker(this);
        sp_ver->setSources("$1->GetVerPos(0,1000)");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp_ver);
        TPlotLightMarker *sp_sum = new TPlotLightMarker(this);
        sp_sum->setSources("$1->GetSum(0,1000)");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp_sum);
    }

    if(qApp->arguments().contains("--attrs")) {
        TPlotLightMarker *sp = new TPlotLightMarker(this);
        sp->setSources("$1/VaPickup;$1/VbPickup;$1/VcPickup;$1/VdPickup");
//        sp->setSources("$1/VaPickup;$1/VbPickup");
        qobject_cast<QVBoxLayout *>(layout())->addWidget(sp);
    }

    foreach(TPlotLightMarker *plot, findChildren<TPlotLightMarker *>()) {
        plot->setPeriod(period);
        plot->setTimeScaleDrawEnabled(false);

        QwtPlotCanvas *c = qobject_cast<QwtPlotCanvas *>(plot->canvas());
        printf("plot canvas paint backing store %d opaque %d HackStyledBackground  %d ImmediatePaint %d\n",
               c->testPaintAttribute(QwtPlotCanvas::BackingStore),
               c->testPaintAttribute(QwtPlotCanvas::Opaque),
              c->testPaintAttribute(QwtPlotCanvas::HackStyledBackground),
               c->testPaintAttribute(QwtPlotCanvas::ImmediatePaint));
    }
}

Bpmqtango::~Bpmqtango()
{
    delete ui;
}
