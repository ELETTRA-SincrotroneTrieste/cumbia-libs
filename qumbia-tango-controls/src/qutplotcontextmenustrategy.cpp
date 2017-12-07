#include "qutplotcontextmenustrategy.h"
#include "quconnectionsettingswidget.h"
#include "qutrealtimeplot.h"
#include "rtconfwidget.h"
#include "cucontext.h"
#include <qutrendplot.h>
#include <quspectrumplot.h>
#include <cumacros.h>
#include <cudata.h>
#include <QMenu>
#include <QAction>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QtDebug>
#include <eplot_configuration_widget.h>

QuTPlotContextMenuStrategy::QuTPlotContextMenuStrategy(bool realtime)
{
    m_realtime = realtime;
}

QMenu *QuTPlotContextMenuStrategy::createMenu(QWidget *w)
{
    QMenu *m = QuPlotBaseContextMenuStrategy::createMenu(w);
    QuPlotBase *plot = qobject_cast<QuPlotBase *>(w);
    QuTPlotSettings *pconf = plot->findChild<QuTPlotSettings *>();
    if(!pconf) /* reuse pconf if possible */
        pconf = new QuTPlotSettings(plot, m_realtime);

    m->findChild<QAction *>("configure")->disconnect();
    QObject::connect(m->findChild<QAction *>("configure"), &QAction::triggered, pconf, &QuTPlotSettings::configure);
    return m;
}

QuTPlotSettings::QuTPlotSettings(QuPlotBase *plot, bool realtime) : QObject(plot)
{
    m_plot = plot;
    m_realtime = realtime;
}

QuTPlotSettings::~QuTPlotSettings()
{
    pdelete("~QuTPlotSettings %p", this);
}

void QuTPlotSettings::configure()
{
    QDialog d(m_plot);
    QVBoxLayout *lo = new QVBoxLayout(&d);
    QGroupBox *gb = new QGroupBox(&d);  // connection settings groupbox
    QVBoxLayout *gblo = new QVBoxLayout(gb); // group box layout
    EPlotConfigurationWidget *base_conf_w = new EPlotConfigurationWidget(m_plot);
    QWidget *connection_conf_w;
    gb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    if(m_realtime && qobject_cast<QuTRealtimePlot *>(m_plot))
    {
        gb->setTitle("Real Time configuration");
        connection_conf_w = new RtConfWidget(qobject_cast<QuTRealtimePlot *>(m_plot));
    }
    else if(!m_realtime)
    {
        gb->setTitle("Connection settings");
        connection_conf_w = new QuConnectionSettingsWidget(m_plot);
        connect(connection_conf_w, SIGNAL(settingsChanged(const CuData&)), this, SLOT(apply(const CuData&)));
    }
    gblo->addWidget(connection_conf_w);
    QTabWidget tab (&d);
    tab.insertTab(0, base_conf_w, "Aspect");
    tab.insertTab(1, gb, "Connection");
    lo->addWidget(&tab);
    d.exec();
}

void QuTPlotSettings::apply(const CuData &da)
{
    if(qobject_cast<QuTrendPlot *>(m_plot))
        qobject_cast<QuTrendPlot *>(m_plot)->getContext()->setOptions(da);
    else if(qobject_cast<QuSpectrumPlot *>(m_plot))
        qobject_cast<QuSpectrumPlot *>(m_plot)->getContext()->setOptions(da);
}
