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
    if(da.containsKey(TTT::QualityColor))  // da.containsKey("qc")
        background = palette[QString::fromStdString(da[TTT::QualityColor].toString())];  // da["qc"]
    if(da.containsKey(TTT::Color))  // da.containsKey("color")
        border = palette[QString::fromStdString(da[TTT::Color].toString())];  // da["color"]

    bool read_ok = !da[TTT::Err].toBool();  // da["err"]


    printf("\e[0;33m ** UPDATE PLOT %s\e[0m\n", da.toString().c_str());

    if(read_ok && d->auto_configure && da[TTT::Type].toString() == "ctrl")  // da["type"]
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
