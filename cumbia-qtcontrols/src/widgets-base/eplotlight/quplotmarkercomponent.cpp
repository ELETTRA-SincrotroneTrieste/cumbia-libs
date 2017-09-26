#include "quplotmarkercomponent.h"
#include <QColor>
#include <QPen>

#include "quplot_base.h"
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>
#include <qwt_scale_map.h>
#include <qwt_picker_machine.h>

void Arrow::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
    //	qDebug() << begin << end;
    double x1, x2, y1, y2;
    QColor penColor(QColor(Qt::lightGray));
    x1 = xMap.transform(begin.x());
    x2 = xMap.transform(end.x());
    y1 = yMap.transform(begin.y());
    y2 = yMap.transform(end.y());
    penColor.setAlpha(120);
    painter->setPen(QColor(Qt::darkGray));
    painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
}

QuPlotMarkerComponent::QuPlotMarkerComponent(QuPlotBase *plot)
{
    init(plot);
}

QString QuPlotMarkerComponent::name() const
{
    return "marker";
}


void QuPlotMarkerComponent::init(QuPlotBase *plot)
{
    const int alpha = 220;
    QColor bgColor(QColor(245,245,245));
    QColor txtColor(QColor(Qt::black));
    QColor bgPen(QColor(Qt::darkGray));
    m_marker = new QwtPlotMarker();
    m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOff, plot->canvas());
    m_picker->setStateMachine(new QwtPickerClickPointMachine());
    QwtText text("", QwtText::PlainText);
    text.setRenderFlags(Qt::AlignCenter | Qt::TextIncludeTrailingSpaces);
    QFont f(plot->font());
    f.setBold(true);
    f.setPointSize(10);
    text.setFont(f);
    bgColor.setAlpha(alpha);
    txtColor.setAlpha(alpha);
    bgPen.setAlpha(alpha);
    text.setColor(txtColor);
    text.setBackgroundBrush(QBrush(bgColor));
    m_marker->setLabel(text);
    m_marker->setLabelAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_marker->setLineStyle(QwtPlotMarker::NoLine);
    m_marker->setValue(0.0, 0.0);
    m_marker->hide();
    m_arrow = new Arrow();
    m_currentClosestCurve = NULL;
    m_currentClosestPoint = -1;
    m_arrow->hide();
}

void QuPlotMarkerComponent::attachToPlot(QuPlotBase *plot)
{
    m_arrow->attach(plot);
    m_marker->attach(plot);
}

void QuPlotMarkerComponent::connectToPlot(QuPlotBase *plot)
{
    QObject::connect(m_picker, SIGNAL(selected(const QPolygon &)), plot, SLOT(showMarker(const QPolygon &)));
}

/*
 * \brief Returns true if the marker has been updated, false otherwise
 *
 * A marker is updated when it's visible and the linked curve is visible.
 */
bool QuPlotMarkerComponent::update(const QuPlotBase *plot)
{
    /* update arrow if the arrow is visible and if the curve has not been removed */
    if(m_arrow->isVisible() && plot->curves().contains(m_currentClosestCurve) &&
            m_currentClosestCurve->isVisible() &&
            m_currentClosestCurve && m_currentClosestPoint != -1)
    {
        double x = m_currentClosestCurve->data()->sample(m_currentClosestPoint).x();
        double y = m_currentClosestCurve->data()->sample(m_currentClosestPoint).y();
        m_arrow->begin = QPointF(x, y);
        //  m_arrow->end = QPointF(m_currentClosestCurve->x(closestPoint)+hoff, up);
        QwtText l = m_marker->label();
        l.setText(markerText(plot, m_currentClosestCurve, m_currentClosestPoint));
        m_marker->setLabel(l);
        return true;
    }
    return false;
}

void QuPlotMarkerComponent::setLabel(const QwtText &text)
{
    m_marker->setLabel(text);
}

void QuPlotMarkerComponent::update(const QuPlotBase *plot,
                                   QwtPlotCurve *closestC,
                                   int closestPointIdx)
{
    double hoff, range, markerW, markerH;
    double xlb, xub, ylb, yub;
    double y = closestC->data()->sample(closestPointIdx).y();
    double x = closestC->data()->sample(closestPointIdx).x();
    xlb = plot->axisScaleDiv(closestC->xAxis()).lowerBound();
    xub = plot->axisScaleDiv(closestC->xAxis()).upperBound();
    ylb = plot->axisScaleDiv(closestC->yAxis()).lowerBound();
    yub = plot->axisScaleDiv(closestC->yAxis()).upperBound();
    range = xub - xlb;

    m_currentClosestCurve = closestC;
    m_currentClosestPoint = closestPointIdx;

    QwtText l = m_marker->label();
    l.setText(markerText(plot, closestC, closestPointIdx));
    m_marker->setLabel(l);

    markerW = l.textSize(l.font()).width();
    markerH = l.textSize(l.font()).height();

    if(x <= xlb + range/3)
    {
        hoff = 0.1*range; // + .2*x;
        m_marker->setLabelAlignment(Qt::AlignRight|Qt::AlignBottom);
    }
    else if(x >= xlb + 2 * range/3)
    {
        hoff = -0.1*range;
        m_marker->setLabelAlignment(Qt::AlignLeft|Qt::AlignBottom);
    }
    else
    {
        hoff = -0.1*range;
        m_marker->setLabelAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    }

    QPointF begin, end, arrowEnd;
    double up = ylb + 0.93 * (yub - ylb);
    begin = QPointF(x, y);
    end = QPointF(x + hoff, up);

    m_marker->setYAxis(closestC->yAxis());
    m_marker->setValue(end);

   /* 1. to obtain the position of the end point of the arrow, transform first the end point into
   * pixel coordinates.
   */
    int arrowXEndPix = plot->transform(closestC->xAxis(), x + hoff);
    int arrowYEndPix = plot->transform(closestC->yAxis(), up);

    /* 2. position end point for the arrow, depending on the area the user clicks.
   * No need to change arrowXEndPix for central area clicks. We are in pixel coordinates
   * and markerWidth and markerHeight are in pixel coordinates: we can sum each other
   */
    if(x <= xlb + range/3)
        arrowXEndPix += markerW/2;
    else if(x >= xlb + 2 * range/3)
        arrowXEndPix -= markerW/2;

    arrowYEndPix += markerH; /* we are summing pixel coords */

    /* 3. Finally set arrow end point, transforming back into plot coordinates */
    arrowEnd = QPointF(plot->invTransform(closestC->xAxis(), arrowXEndPix),
                       plot->invTransform(closestC->yAxis(), arrowYEndPix));
    m_arrow->setYAxis(closestC->yAxis());
    m_arrow->begin = begin;
    m_arrow->end = arrowEnd;
}


void QuPlotMarkerComponent::hide()
{
    m_marker->hide();
    m_arrow->hide();
}

void QuPlotMarkerComponent::show()
{
    m_marker->show();
    m_arrow->show();
}

bool QuPlotMarkerComponent::isVisible() const
{
    return m_marker->isVisible();
}

QwtPlotCurve *QuPlotMarkerComponent::currentClosestCurve() const
{
    return m_currentClosestCurve;
}

int QuPlotMarkerComponent::currentClosestPoint() const
{
    return m_currentClosestPoint;
}

QString QuPlotMarkerComponent::markerText(const QuPlotBase *plot,
                                          const QwtPlotCurve *curve,
                                          const int index)
{
    QString s;
    double y, x, y1;
    if(curve != NULL)
    {
        x = curve->data()->sample(index).x();
        y = curve->data()->sample(index).y();
        /* place the x coordinate label, taken from the x axis scale */
        const QwtScaleDraw *scaleDraw = plot->axisScaleDraw(curve->xAxis());
        s += QString("x: %1\n").arg(scaleDraw->label(x).text());
        /* get overlapping curves in point x,y and add to the marker the y values */
        QSet<QwtPlotCurve *> overlappingCurves = intersectingCurves(plot, x, y, curve);
        /* add the current curve to the overlapping set */
        overlappingCurves.insert((QwtPlotCurve *)(curve)); /* remove constness */
        foreach(QwtPlotCurve *pc, overlappingCurves.values())
        {
            y1 = pc->data()->sample(index).y();
            /* place the curve title on the marker text first */
            if(pc->title() != QwtText(QString()))
                s += QString("%1: ").arg(pc->title().text());
            s += QString("%1").arg(y1, 0, 'g', 5) + "\n";
        }
    }
    return s;
}

QSet<QwtPlotCurve *> QuPlotMarkerComponent::intersectingCurves(const QuPlotBase *plot,
                                                               const double x,
                                                               const double y,
                                                               const QwtPlotCurve* curve)
{
    QSet<QwtPlotCurve *> overlappingCurvesSet;
    double vx, vy;
    foreach(QwtPlotCurve *c, plot->curves())
    {
        /* curves not visible are not added to the overlapping set */
        if(c != curve && c->isVisible())
        {
            for(size_t i = 0; i < c->dataSize(); i++)
            {
                vx = c->data()->sample(i).x();
                vy = c->data()->sample(i).y();
                if(x == vx && y == vy)
                {
                    overlappingCurvesSet.insert(c);
                    break;
                }
            }
        }
    }
    return overlappingCurvesSet;
}
