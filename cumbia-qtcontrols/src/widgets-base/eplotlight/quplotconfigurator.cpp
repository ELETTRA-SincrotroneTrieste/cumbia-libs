#include "quplotconfigurator.h"
#include <QString>
#include <QCoreApplication>
#include <QSettings>
#include <quplotcurve.h>
#include <qupalette.h>
#include <cumacros.h>

QuPlotConfigurator::QuPlotConfigurator()
{

}

bool QuPlotConfigurator::hasCustomConf(const QuPlotCurve *c) const {
    QSettings s;
    const QString& id = m_get_id(c);
    return s.contains(id);
}

void QuPlotConfigurator::save(const QuPlotCurve *c)
{
    QSettings s;
    const QString& id = m_get_id(c);
    s.setValue(id + "/color", c->pen().color());
    s.setValue(id + "/color/alpha", c->pen().color().alpha());
    s.setValue(id + "/style", c->style());
    s.setValue(id + "/width", c->pen().width());
}

void QuPlotConfigurator::configure(QuPlotCurve *c, int curves_cnt)
{
    QSettings s;
    const QString& id = m_get_id(c);
    if(s.contains(id)) {
        printf("\e[1,33m loading from settings %s\e[0m\n", qstoc(id));
        /* choose a nice curve color or load it from QSettings, if defined for the application name and
         * curve name.
         */
        QColor curveColor = s.value(id + "/color", c->pen().color()).value<QColor>();
        int alpha = s.value(id + "/color/alpha", c->pen().color().alpha()).toInt();
        int curveStyle = s.value(id + "/style", c->style()).toInt();
        double penWidth = s.value(id + "/width", c->pen().widthF()).toDouble();
//        bool displayYValues = s.value(id + "/show-values", false).toBool();
        curveColor.setAlpha(alpha);
        QPen curvePen(curveColor);
        curvePen.setWidth(penWidth);
        c->setStyle((QwtPlotCurve::CurveStyle) curveStyle);
        c->setPen(curvePen);
//        c->setProperty("showYValuesEnabled", displayYValues);
    }
    else {
        printf("\e[1,31m settings %s UNFOUND\e[0m\n", qstoc(id));

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

QString QuPlotConfigurator::m_get_id(const QuPlotCurve *c) const {
    const QString& ct = c->title().text();
    return QString("%1/plot_curves/%2").arg(QCoreApplication::instance()->applicationName()).arg(ct);
}
