#ifndef QUPLOTCONFIGURATOR_H
#define QUPLOTCONFIGURATOR_H

class QuPlotCurve;
class QuPlotBase;

#include <QString>

class QuPlotConfigurator
{
public:
    QuPlotConfigurator();

    bool hasCustomConf(const QString &plotnam, const QuPlotCurve *c) const;
    void save(const QString &plot_name, const QuPlotCurve *c);
    void save( QuPlotBase *plot);
    void configure_curve(const QuPlotBase *plot, QuPlotCurve *c, int curves_cnt);
    void configure( QuPlotBase *plot);
    void clearSettings(QuPlotBase *plot) const;

private:
    QString m_get_id(const QString &plot_name, const QuPlotCurve *c) const;
};

#endif // QUPLOTCONFIGURATOR_H
