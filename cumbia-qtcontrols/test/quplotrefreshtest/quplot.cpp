/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "quplot.h"
#include "circularbuffer.h"
#include "quplotsettings.h"

#include <QwtPainter>
#include <QwtPlotCanvas>
#include <QwtPlotGrid>
#include <quplotcurve.h>
#include <QwtPlotLayout>
#include <QwtScaleWidget>
#include <QwtScaleDraw>
#include <QwtMath>
#include <QtDebug>

#include <cumacros.h>

#ifndef QWT_NO_OPENGL

#if QT_VERSION >= 0x050400
#define USE_OPENGL_WIDGET 1
#endif

#if USE_OPENGL_WIDGET
#include <QwtPlotOpenGLCanvas>
#else
#include <QwtPlotGLCanvas>
#endif
#endif

static double wave( double x )
{
    const double period = 1.0;
    const double c = 5.0;

    double v = std::fmod( x, period );

    const double amplitude = qAbs( x - qRound( x / c ) * c ) / ( 0.5 * c );
    v = amplitude * std::sin( v / period * 2 * M_PI );

    return v;
}

static double noise( double x)
{
    return 2.0 * ( qwtRand() / ( static_cast< double >( RAND_MAX ) + 1 ) ) - 1.0;
}

QuPlot::QuPlot( QWidget* parent )
    : QuSpectrumPlot( parent )
    , m_interval( 10.0 ) // seconds
    , m_timerId( -1 )
{
    // Assign a title
    setTitle( "Cumbia Plots: Testing Refresh Rates" );

    /// remove this section.
    /// Use QuPlotBase configuration
    ///
    // QwtPlotCanvas* canvas = new QwtPlotCanvas();
    // canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    // canvas->setLineWidth( 1 );
    // canvas->setPalette( Qt::white );

    // setCanvas( canvas );

    // alignScales();

    // // Insert grid
    // m_grid = new QwtPlotGrid();
    // m_grid->attach( this );

    // Insert curve. Use QuPlotBase addCurve
    addCurve("Data Moving Right");
    m_curve = curve("Data Moving Right");
    m_circularbuf = new CircularBuffer( m_interval, 10 ); // for QwtSeriesData

    // Axis
    setAxisTitle( QwtAxis::XBottom, "Seconds" );
    setAxisScale( QwtAxis::XBottom, -m_interval, 0.0 );

    setAxisTitle( QwtAxis::YLeft, "Values" );
    setAxisScale( QwtAxis::YLeft, -1.0, 1.0 );

    m_elapsedTimer.start();
    const QList<QwtPlotItem *> &il = itemList(QwtPlotItem::Rtti_PlotGrid);
    if(il.size() > 0) {
        m_grid = static_cast<QwtPlotGrid *>(il.first());
    }
    // setSettings( m_settings );
}

void QuPlot::setSettings( const QuPlotSettings& s )
{
    if ( m_timerId >= 0 )
        killTimer( m_timerId );

    m_timerId = startTimer( s.updateInterval );

    printf("setSettings grid is %p\n", m_grid);
    m_grid->setPen( s.grid.pen );
    m_grid->setVisible( s.grid.pen.style() != Qt::NoPen );

    if(s.setDataType == QuPlotSettings::VectorSetData) {
        m_curve->setData(nullptr); // deletes circular buffer
        m_circularbuf = new CircularBuffer(m_interval, 10);
    } else {
        m_curve->setData(m_circularbuf);
    }

    if ( s.curve.numPoints != m_circularbuf->size() ||
        s.curve.functionType != m_settings.curve.functionType )
    {
        switch( s.curve.functionType )
        {
        case QuPlotSettings::Wave:
            m_circularbuf->setFunction( wave );
            break;
        case QuPlotSettings::Noise:
            m_circularbuf->setFunction( noise );
            break;
        default:
            m_circularbuf->setFunction( NULL );
        }

        m_circularbuf->fill( m_interval, s.curve.numPoints );
    }



    m_curve->setPen( s.curve.pen );
    m_curve->setBrush( s.curve.brush );

    m_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons,
                               s.curve.paintAttributes & QwtPlotCurve::ClipPolygons );
    m_curve->setPaintAttribute( QwtPlotCurve::FilterPoints,
                               s.curve.paintAttributes & QwtPlotCurve::FilterPoints );
    m_curve->setPaintAttribute( QwtPlotCurve::FilterPointsAggressive,
                               s.curve.paintAttributes & QwtPlotCurve::FilterPointsAggressive );

    m_curve->setRenderHint( QwtPlotItem::RenderAntialiased,
                           s.curve.renderHint & QwtPlotItem::RenderAntialiased );

    setOpenGL(s.canvas.openGL);

    if(!s.canvas.openGL) {
        QwtPlotCanvas* plotCanvas = qobject_cast< QwtPlotCanvas* >( canvas() );
        plotCanvas->setAttribute( Qt::WA_PaintOnScreen, s.canvas.paintOnScreen );
        plotCanvas->setPaintAttribute(
            QwtPlotCanvas::BackingStore, s.canvas.useBackingStore );
        plotCanvas->setPaintAttribute(
            QwtPlotCanvas::ImmediatePaint, s.canvas.immediatePaint );
    }

    // * regardless the openGL setting (see refreshplot's Plot.cpp)
    // En/Disable line splitting for the raster paint engine.
    // In some Qt versions the raster paint engine paints polylines of many points much faster when
    // they are split in smaller chunks: f.e all supported Qt versions >= Qt 5.0 when drawing an antialiased polyline with a pen width >=2.
    // Also the raster paint engine has a nasty bug in many versions ( Qt 4.8 - ... ) for short lines ( https://codereview.qt-project.org/#/c/99456 ), that is worked around in this mode.
    // The default setting is true.
    //
    // https://qwt.sourceforge.io/class_qwt_painter.html#a174bca411198c64dfff828a15d80ddfb
    QwtPainter::setPolylineSplitting( s.curve.lineSplitting );

    m_settings = s;

}

void QuPlot::timerEvent( QTimerEvent* )
{
    m_circularbuf->setReferenceTime( m_elapsedTimer.elapsed() / 1000.0 );
    if(m_settings.setDataType == QuPlotSettings::VectorSetData) {
        QVector<double> x, y;
        m_circularbuf->getData(x, y);
        setData("Data Moving Right", x, y); // deletes m_circularbuf1
    }
    if(m_settings.updateType == QuPlotSettings::QuPlotRefresh)
        refresh();
    else if ( m_settings.updateType == QuPlotSettings::RepaintCanvas )
    {
        // the axes in this example doesn't change. So all we need to do
        // is to repaint the canvas.

        QMetaObject::invokeMethod( canvas(), "replot", Qt::DirectConnection );
    }
    else
    {
        replot();
    }
}
