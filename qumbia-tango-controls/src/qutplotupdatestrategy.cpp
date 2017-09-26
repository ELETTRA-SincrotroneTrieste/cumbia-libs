#include "qutplotupdatestrategy.h"
#include <QColor>
#include <cudata.h>
#include <cumacros.h>
#include <cuvariant.h>
#include <QWidget>
#include <qupalette.h>
#include <qutrendplot.h>

class QuEpicsPlotUpdateStrategyPrivatevate
{
public:
    bool auto_configure;
};

QuTPlotUpdateStrategy::QuTPlotUpdateStrategy(bool auto_configure)
{
    d = new QuEpicsPlotUpdateStrategyPrivatevate;
    d->auto_configure = auto_configure;
}

QuTPlotUpdateStrategy::~QuTPlotUpdateStrategy()
{
    delete d;
}

void QuTPlotUpdateStrategy::update(const CuData &da, QWidget *widget)
{
    QColor background, border;
    QuPalette palette;
    if(da.containsKey("quality_color"))
        background = palette[QString::fromStdString(da["quality_color"].toString())];
    if(da.containsKey("success_color"))
        border = palette[QString::fromStdString(da["success_color"].toString())];

    bool read_ok = !da["err"].toBool();

    if(read_ok && d->auto_configure && da.containsKey("mode")
            && da["mode"].toString() == "ATTCONF")
    {
        m_configure(da, widget);
    }
    else
    {
        static_cast<QuPlotBase *>(widget)->update(da);
    }

    // use border and background maybe ?
}

void QuTPlotUpdateStrategy::m_fillFromHistory(const CuVariant &x, const CuVariant& y)
{
    if(x.getSize() == y.getSize() &&  x.getFormat() == y.getFormat() &&
            x.getType() == y.getType() && x.getFormat() == CuVariant::Vector)
    {
        std::vector<double> v = y.toDoubleVector();
        std::vector<double> xtime = x.toDoubleVector();
        if(v.size() == xtime.size())
            printf("----> NEED FILL FROM HISTORY in QuTrendPlot <----\n");
    }
}

void QuTPlotUpdateStrategy::m_configure(const CuData &da, QWidget* widget)
{
    CuVariant m, M;
    m = da["min_value"];
    M = da["max_value"];
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

    /* data could possibly contain a piece of the value's history */
    if(da.containsKey("history_y") && da.containsKey("history_x"))
        m_fillFromHistory(da["history_x"], da["history_y"]);
}
