#include "quplotzoomcomponent.h"
#include "quplot_base.h"

#include <QPen>
#include <QtDebug>
#include <cumacros.h>

QuPlotZoomComponent::QuPlotZoomComponent(QuPlotBase *plot)
    : m_plot(plot)
{
    pretty_pri("creating zoom component %p", this);
    init(plot);
}

QuPlotZoomComponent::~QuPlotZoomComponent() {

}

QString QuPlotZoomComponent::name() const
{
    return "zoom";
}

void QuPlotZoomComponent::init(QuPlotBase *plot)
{
    if(plot->canvas()) {
        ScrollZoomer *zoomer = plot->canvas()->findChild<ScrollZoomer *>();
        if(!zoomer) {
            zoomer = new ScrollZoomer(plot->canvas());
            zoomer->setRubberBandPen(QPen(QColor(Qt::gray), 1, Qt::SolidLine));
            zoomer->setTrackerPen(QPen(Qt::gray));
            zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ShiftModifier);
            zoomer->setZoomBase();
        }
        pretty_pri("zoomer %p init on canvas %p", zoomer, plot->canvas());
    }
}


bool QuPlotZoomComponent::inZoom() const {
    ScrollZoomer *zoomer = m_plot->findChild<ScrollZoomer *>();
    return zoomer && zoomer->zoomRectIndex() != 0;
}

void QuPlotZoomComponent::changeRect(int axisId, double delta_lower, double delta_upper)
{
    ScrollZoomer *z = m_plot->findChild<ScrollZoomer *>();
    if(z) {
        QRectF r = z->zoomBase();
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

        qDebug() << __FUNCTION__ << "zoom from " << z->zoomBase() << "to" << r
                 << "delta left" << delta_lower << " delta rite " << delta_upper;
        z->setZoomBase(r);
    }
}

void QuPlotZoomComponent::setZoomBase(bool do_replot)
{
    if(m_plot->findChild<ScrollZoomer *>())
        m_plot->findChild<ScrollZoomer *>()->setZoomBase(do_replot);
}

void QuPlotZoomComponent::setZoomBase(const QRectF &r) {
    if(m_plot->findChild<ScrollZoomer *>())
        m_plot->findChild<ScrollZoomer *>()->setZoomBase(r);
}

QRectF QuPlotZoomComponent::zoomBase() const {
    return m_plot->findChild<ScrollZoomer *>() ?
               m_plot->findChild<ScrollZoomer *>()->zoomBase() : QRectF();
}

QRectF QuPlotZoomComponent::zoomRect() const {
    return m_plot->findChild<ScrollZoomer *>()->zoomRect();
}

void QuPlotZoomComponent::canvasChanged(QuPlotBase *plot) {
    pretty_pri("\e[1;31mcanvas changed!\e[0m");
    init(plot);
}

void QuPlotZoomComponent::attachToPlot(QuPlotBase *plot)
{
    Q_UNUSED(plot);
}

void QuPlotZoomComponent::connectToPlot(QuPlotBase *plot)
{
    //    QObject::connect(m_zoomer, SIGNAL(zoomHint()), plot, SLOT(displayZoomHint()));
    //    QObject::connect(m_zoomer, SIGNAL(removeZoomHint()), plot, SLOT(eraseZoomHint()));
    QObject::connect(m_plot->findChild<ScrollZoomer *>(), SIGNAL(zoomed(const QRectF&) ), plot, SLOT(plotZoomed(const QRectF&) ) );
}
