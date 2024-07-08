#include "qutplotupdatestrategy.h"
#include <QColor>
#include <cudata.h>
#include <cumacros.h>
#include <cuvariant.h>
#include <QWidget>
#include <qupalette.h>
#include <qutrendplot.h>

/*! @private
 */
class QuTPlotUpdateStrategyPrivate
{
public:
    bool auto_configure;
};

/*! \brief The class constructor
 *
 * @param auto_configure true the plot uses Tango attribute properties to configure
 *        lower and upper bounds
 * @param auto_configure false no auto configuration is performed. Upper and lower
 *        bounds must be set manually or the plot scales must be put in autoscale
 *        mode
 */
QuTPlotUpdateStrategy::QuTPlotUpdateStrategy(bool auto_configure)
{
    d = new QuTPlotUpdateStrategyPrivate;
    d->auto_configure = auto_configure;
}

/*! \brief the class destructor
 */
QuTPlotUpdateStrategy::~QuTPlotUpdateStrategy()
{
    delete d;
}

/*! \brief implementation of the QuWidgetUpdateStrategyI::update method mainly
 *         aimed at setting up the plot default lower and upper y scale bounds
 *
 * @param da data from the attribute (command)
 * @param widget the QuPlotBase that will be configured (the target of the stragegy
 *        pattern).
 *
 * if the CuData bundle contains the key "type" with value "property", then the
 * plot is configured taking minimum and maximum values from the Tango attribute
 * properties. Otherwise, QuPlotBase::update is called.
 *
 */
void QuTPlotUpdateStrategy::update(const CuData &da, QWidget *widget)
{
    QColor background, border;
    QuPalette palette;
    if(da.containsKey(TTT::QualityColor))  // da.containsKey("qc")
        background = palette[QString::fromStdString(da[TTT::QualityColor].toString())];  // da["qc"]
    if(da.containsKey(TTT::Color))  // da.containsKey("color")
        border = palette[QString::fromStdString(da[TTT::Color].toString())];  // da["color"]

    bool read_ok = !da[TTT::Err].toBool();  // da["err"]

    if(read_ok && d->auto_configure && da.containsKey(TTT::Type)  // da.containsKey("type")
            && da[TTT::Type].toString() == "property")  // da["type"]
    {
        m_configure(da, widget);
    }
    else
    {
        static_cast<QuPlotBase *>(widget)->update(da);
    }

    // use border and background maybe ?
}

/*! @private
 */
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

/*! @private
 */
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
