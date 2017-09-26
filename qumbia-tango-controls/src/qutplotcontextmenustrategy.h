#ifndef QUTPLOTCONTEXTMENUSTRATEGY_H
#define QUTPLOTCONTEXTMENUSTRATEGY_H

#include <quplotbasecontextmenustrategy.h>
#include <QWidget>

class QDialog;
class QuPlotBase;
class CuData;

class QuTPlotContextMenuStrategy : public QuPlotBaseContextMenuStrategy
{
public:
    QuTPlotContextMenuStrategy(bool realtime = false);
    // QuWidgetContextMenuStrategyI interface

    QMenu *createMenu(QWidget *w);

private:
    bool m_realtime;

};

class QuTPlotSettings : public QObject
{
    Q_OBJECT
public:
    QuTPlotSettings(QuPlotBase *plot, bool realtime = false);

    virtual ~QuTPlotSettings();

public slots:
    void configure();

    void apply(const CuData& da);

private:
    QuPlotBase *m_plot;
    bool m_realtime;
};

#endif // QUTPLOTCONFIGURATION_H
