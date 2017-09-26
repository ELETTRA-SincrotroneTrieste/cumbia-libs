#include "quplotzoomcomponent.h"
#include "quplot_base.h"

#include <QtDebug>

QuPlotZoomComponent::QuPlotZoomComponent(QuPlotBase *plot)
{
    init(plot);
}

QuPlotZoomComponent::~QuPlotZoomComponent()
{
    delete m_zoomer;
}

QString QuPlotZoomComponent::name() const
{
    return "zoom";
}

void QuPlotZoomComponent::init(QuPlotBase *plot)
{
    m_zoomer = new ScrollZoomer(plot->canvas());
    m_zoomer->setRubberBandPen(QPen(QColor(Qt::gray), 1, Qt::SolidLine));
    m_zoomer->setTrackerPen(QPen(Qt::gray));
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ShiftModifier);
    m_zoomer->setZoomBase();
}


bool QuPlotZoomComponent::inZoom() const
{
    return m_zoomer->zoomRectIndex() != 0;
}

void QuPlotZoomComponent::changeRect(int axisId, double delta_lower, double delta_upper)
{
    QRectF r = m_zoomer->zoomBase();
    if(axisId == QwtPlot::xBottom || axisId == QwtPlot::xTop)
    {
        r.moveRight(r.right()+ delta_upper); /* just move */
        r.moveLeft(r.left() + delta_lower);
    }
    else
    {
        r.moveTop(r.top()+ delta_upper); /* just move */
        r.moveBottom(r.bottom() + delta_lower);
    }

    qDebug() << __FUNCTION__ << "zoom from " << m_zoomer->zoomBase() << "to" << r
             << "delta left" << delta_lower << " delta rite " << delta_upper;
    m_zoomer->setZoomBase(r);
}

void QuPlotZoomComponent::setZoomBase(bool do_replot)
{
    m_zoomer->setZoomBase(do_replot);
}

void QuPlotZoomComponent::setZoomBase(const QRectF &r)
{
    m_zoomer->setZoomBase(r);
}

QRectF QuPlotZoomComponent::zoomBase() const
{
    return m_zoomer->zoomBase();
}

QRectF QuPlotZoomComponent::zoomRect() const
{
    return m_zoomer->zoomRect();
}

void QuPlotZoomComponent::attachToPlot(QuPlotBase *plot)
{
    Q_UNUSED(plot);
}

void QuPlotZoomComponent::connectToPlot(QuPlotBase *plot)
{
    QObject::connect(m_zoomer, SIGNAL(zoomHint()), plot, SLOT(displayZoomHint()));
    QObject::connect(m_zoomer, SIGNAL(removeZoomHint()), plot, SLOT(eraseZoomHint()));
    QObject::connect(m_zoomer, SIGNAL(zoomed(const QRectF&) ), plot, SLOT(plotZoomed(const QRectF&) ) );
}
