/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "quplotsettings.h"

#include <quspectrumplot.h>
#include <QElapsedTimer>

class QwtPlotGrid;
class QwtPlotCurve;
class CircularBuffer;

class QuPlot : public QuSpectrumPlot
{
    Q_OBJECT

  public:
    QuPlot( QWidget* = NULL );

  public Q_SLOTS:
    void setSettings( const QuPlotSettings& );

  protected:
    virtual void timerEvent( QTimerEvent* ) QWT_OVERRIDE;

  private:
    void alignScales();

    QwtPlotGrid* m_grid;
    QwtPlotCurve* m_curve;

    QElapsedTimer m_elapsedTimer;
    double m_interval;

    int m_timerId;
    CircularBuffer *m_circularbuf;

    QuPlotSettings m_settings;
};
