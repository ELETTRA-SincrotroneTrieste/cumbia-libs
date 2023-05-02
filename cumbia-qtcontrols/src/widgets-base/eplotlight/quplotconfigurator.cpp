#include "quplotconfigurator.h"
#include <QString>
#include <QPen>
#include <qwt_text.h>
#include <quplotcurve.h>
#include <QCoreApplication>
#include <QSettings>
#include <quplotcurve.h>
#include <qupalette.h>
#include <cumacros.h>
#include "quplot_base.h"
#include "quplot_base.h"
#include "quplotaxescomponent.h"

QuPlotConfigurator::QuPlotConfigurator()
{

}

bool QuPlotConfigurator::hasCustomConf(const QString& plotnam, const QuPlotCurve *c) const {
    QSettings s;
    const QString& id = m_get_id(plotnam, c);
    return s.contains(id);
}

void QuPlotConfigurator::save( QuPlotBase *plot) {
    QSettings s("elettra.eu", qApp->applicationName());
    const QString& id = m_get_id(plot->objectName(), nullptr);
    s.beginGroup(id);
    if(plot->xAxisAutoscaleEnabled(QwtPlot::xBottom)) {
        s.setValue("axes/xbot/autoscale", true);
    }
    else {
        s.setValue("axes/xbot/autoscale", false);
        s.setValue("axes/bounds/xbot/xlow", plot->xLowerBound(QwtPlot::xBottom));
        s.setValue("axes/bounds/xbot/xup", plot->xUpperBound(QwtPlot::xBottom));
    }
    if(plot->yAxisAutoscaleEnabled(QwtPlot::yLeft)) {
        s.setValue("axes/yleft/autoscale", true);
    }
    else {
        s.setValue("axes/yleft/autoscale", false);
        s.setValue("axes/bounds/yleft/ylow", plot->yLowerBound(QwtPlot::yLeft));
        s.setValue("axes/bounds/yleft/yup", plot->yUpperBound(QwtPlot::yLeft));
    }
    // log scale
    s.setValue("axes/xbot/logscale", plot->xAxisLogScale(QwtPlot::xBottom));
    s.setValue("axes/yleft/logscale", plot->yAxisLogScale(QwtPlot::yLeft));

    // autoscale "adjustment"
    s.setValue("axes/yleft/autoscale.adjust/value", plot->yAutoscaleAdjustment());
    s.endGroup();
}

void QuPlotConfigurator::save(const QString& plot_name, const QuPlotCurve *c)
{
    QSettings s("elettra.eu", qApp->applicationName());
    const QString& id = m_get_id(plot_name, c);
    s.beginGroup(id);
    s.setValue("pen/color", c->pen().color());
    s.setValue("pen/color/alpha", c->pen().color().alpha());
    s.setValue("style", c->style());
    s.setValue("pen/width", c->pen().width());
    s.endGroup();
}

void QuPlotConfigurator::configure_curve(const QuPlotBase *plot, QuPlotCurve *c, int curves_cnt)
{
    QSettings s("elettra.eu", qApp->applicationName());
    const QString& id = m_get_id(plot->objectName(), c);
    QStringList groups = s.childGroups();
    int idx = groups.indexOf(id);
    if(c != nullptr && idx >= 0) {
        s.beginGroup(id);
        printf("\e[1,33m loading from settings %s\e[0m\n", qstoc(id));
        /* choose a nice curve color or load it from QSettings, if defined for the application name and
         * curve name.
         */
        QColor curveColor = s.value("pen/color", c->pen().color()).value<QColor>();
        int alpha = s.value("pen/color/alpha", c->pen().color().alpha()).toInt();
        int curveStyle = s.value("pen/style", c->style()).toInt();
        double penWidth = s.value("pen/width", c->pen().widthF()).toDouble();
        //        bool displayYValues = s.value(id + "/show-values", false).toBool();
        curveColor.setAlpha(alpha);
        QPen curvePen(curveColor);
        curvePen.setWidth(penWidth);
        c->setStyle((QwtPlotCurve::CurveStyle) curveStyle);
        c->setPen(curvePen);
        //        c->setProperty("showYValuesEnabled", displayYValues);
        s.endGroup();
    }
    else if(c != nullptr) {
        QuPalette palette;
        QStringList colors = QStringList() << "dark_green" << "blue" << "violet"
                                           << "red" << "black" << "light_gray" << "yellow" <<  "green" << "gray"
                                           << "orange" << "pink" << "dark_red";
        QString color_nam = colors.at(curves_cnt % colors.size());
        QColor curveColor = palette.value(color_nam);
        QPen curvePen(curveColor);
        c->setPen(curvePen);
    }
}

void QuPlotConfigurator::configure(QuPlotBase *plot) {
    QSettings s("elettra.eu", qApp->applicationName());
    const QString& id = m_get_id(plot->objectName(), nullptr);
    QStringList groups = s.childGroups();
    int idx = groups.indexOf(id);
    if(idx >= 0) {
        QSettings s("elettra.eu", qApp->applicationName());
        const QString& id = m_get_id(plot->objectName(), nullptr);
        s.beginGroup(id);
        // set x scale bounds if not autoscale mode
        QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(plot->getComponent("axes"));
        if(s.value("axes/xbot/autoscale", true).toBool() != plot->xAxisAutoscaleEnabled(QwtPlot::xBottom)) {
            double xl = s.value("axes/bounds/xbot/xlow", 0.0).toDouble();
            double xu = s.value("axes/bounds/xbot/xup", 0.0).toDouble();
            if(xl != plot->xLowerBound() || xu != plot->xUpperBound()) {
                axes_c->setManualBounds(plot, QwtPlot::xBottom, xl, xu);
            }
        }
        if(s.value("axes/yleft/autoscale", true).toBool() != plot->yAxisAutoscaleEnabled(QwtPlot::yLeft)) {
            plot->setYAxisAutoscaleEnabled(s.value("axes/yleft/autoscale", true).toBool(), QwtPlot::yLeft);
        }
        // set y scale bounds if not autoscale mode
        if(!plot->yAxisAutoscaleEnabled(QwtPlot::yLeft)) {
            double l = s.value("axes/bounds/yleft/ylow", 0.0).toDouble();
            double u = s.value("axes/bounds/yleft/yup", 0.0).toDouble() ;
            if(l != plot->yLowerBound() || u != plot->yUpperBound()) {
                axes_c->setManualBounds(plot, QwtPlot::yLeft, l, u);
            }
        }
        // need to replot if used axes_c->setManualBounds
        if(!plot->yAxisAutoscaleEnabled(QwtPlot::yLeft) || !plot->xAxisAutoscaleEnabled(QwtPlot::xBottom))
            plot->replot();

        // log scale
        if(s.value("axes/xbot/logscale", false).toBool() != plot->xAxisLogScale(QwtPlot::xBottom))
            plot->setXAxisLogScale(s.value("axes/xbot/logscale", false).toBool(), QwtPlot::xBottom);
        if(s.value("axes/yleft/logscale", false).toBool() != plot->yAxisLogScale(QwtPlot::yLeft))
            plot->setYAxisLogScale(s.value("axes/yleft/logscale", false).toBool(), QwtPlot::yLeft);

        // autoscale "adjustment"
        if(s.value("axes/yleft/autoscale.adjust/value", 0.0).toDouble() != plot->yAutoscaleAdjustment())
            plot->setYAutoscaleAdjustment(s.value("axes/yleft/autoscale.adjust/value", 0.0).toDouble());
        s.endGroup();
    }
}

void QuPlotConfigurator::clearSettings(QuPlotBase *plot) const {
    QSettings s("elettra.eu", qApp->applicationName());
    QString plotnam = plot->objectName().length() > 0 ? plot->objectName() : "plot";
    foreach(const QString& g, s.childGroups()) {
        if(g.startsWith(qApp->applicationName() + "." + plotnam)) {
            s.remove(g);
        }
    }
}

QString QuPlotConfigurator::m_get_id(const QString& plot_name, const QuPlotCurve *c) const {
    QString s = QString("%1.%2").arg(QCoreApplication::instance()->applicationName()).arg(plot_name.length() > 0 ? plot_name : "plot");
    if(c)  s += ".curves." + c->title().text();
    s.replace('/', '.');
    return s;
}
