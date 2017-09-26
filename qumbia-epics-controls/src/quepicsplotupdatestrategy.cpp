#include "quepicsplotupdatestrategy.h"
#include <QColor>
#include <cudata.h>
#include <cumacros.h>
#include <cuvariant.h>
#include <QWidget>
#include <qupalette.h>
#include <qutrendplot.h>

class QuEpicsPlotUpdateStrategyPrivate
{
public:
    bool auto_configure;
};

QuEpicsPlotUpdateStrategy::QuEpicsPlotUpdateStrategy(bool auto_configure)
{
    d = new QuEpicsPlotUpdateStrategyPrivate;
    d->auto_configure = auto_configure;
}

QuEpicsPlotUpdateStrategy::~QuEpicsPlotUpdateStrategy()
{
    delete d;
}

void QuEpicsPlotUpdateStrategy::update(const CuData &da, QWidget *widget)
{
    QColor background, border;
    QuPalette palette;
    if(da.containsKey("quality_color"))
        background = palette[QString::fromStdString(da["quality_color"].toString())];
    if(da.containsKey("success_color"))
        border = palette[QString::fromStdString(da["success_color"].toString())];

    bool read_ok = !da["err"].toBool();


    printf("\e[0;33m ** UPDATE PLOT %s\e[0m\n", da.toString().c_str());

    if(read_ok && d->auto_configure && da["type"].toString() == "ctrl")
    {
        m_configure(da, widget);
    }
    else
    {
        static_cast<QuPlotBase *>(widget)->update(da);
    }

    // use border and background maybe ?
}

void QuEpicsPlotUpdateStrategy::m_configure(const CuData &da, QWidget* widget)
{
    CuVariant m, M;
    m = da["lower_disp_limit"];
    M = da["upper_disp_limit"];
    bool okl, oku;
    double lb, ub;
    QString min = QString::fromStdString(m.toString());
    QString max = QString::fromStdString(M.toString());
    lb = min.toDouble(&okl);
    ub = max.toDouble(&oku);

    if(okl && oku)
    {
        QuPlotBase *plot = qobject_cast<QuPlotBase*>(widget);
        double current_def_lb = plot->defaultLowerBound(QwtPlot::yLeft);
        double current_def_ub = plot->defaultUpperBound(QwtPlot::yLeft);
        if(current_def_lb > lb)
            current_def_lb = lb;
        if(current_def_ub < ub)
            current_def_ub = ub;
        cuprintf("\e[1;35mbounds from auto %f %f\n\e[0m\n", current_def_lb, current_def_ub);
        plot->setDefaultBounds(current_def_lb, current_def_ub, QwtPlot::yLeft);
    }


}
