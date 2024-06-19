#include "quplotmarkercomponent.h"
#include "quplotcurve.h"
#include <QColor>
#include <QPen>
#include <QPainter>
#include <QSet>

#include "quplot_base.h"
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>
#include <qwt_scale_map.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_curve.h>
#include <cumacros.h>

void Arrow::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
    //	qDebug() << begin << end;
    double x1, x2, y1, y2;
    QColor penColor(Qt::lightGray);
    x1 = xMap.transform(begin.x());
    x2 = xMap.transform(end.x());
    y1 = yMap.transform(begin.y());
    y2 = yMap.transform(end.y());
    penColor.setAlpha(120);
    painter->setPen(QColor(Qt::darkGray));
    painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
}

class QuPlotMarkerComponentPrivate {
public:
    QwtPlotMarker *m_marker;
    QwtPlotPicker *m_picker;
    Arrow *m_arrow;
    QwtPlotCurve *m_currentClosestCurve;
    int m_currentClosestPoint;
    QString xText, yText;
    QuPlotMarkerFormat_I *formatter;
};

QuPlotMarkerComponent::QuPlotMarkerComponent(QuPlotBase *plot)
{
    d = new QuPlotMarkerComponentPrivate;
    init(plot);
}

QuPlotMarkerComponent::~QuPlotMarkerComponent()
{
    if(d->formatter)
        delete d->formatter;
    delete d;
}

QString QuPlotMarkerComponent::name() const {
    return "marker";
}


void QuPlotMarkerComponent::init(QuPlotBase *plot)
{
    const int alpha = 220;
    QColor bgColor(QColor(245,245,245));
    QColor txtColor(Qt::black);
    QColor bgPen(Qt::darkGray);
    d->m_marker = new QwtPlotMarker();
    d->m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOff, plot->canvas());
    d->m_picker->setStateMachine(new QwtPickerClickPointMachine());
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
    d->m_marker->setLabel(text);
    d->m_marker->setLabelAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->m_marker->setLineStyle(QwtPlotMarker::NoLine);
    d->m_marker->setValue(0.0, 0.0);
    d->m_marker->hide();
    d->m_arrow = new Arrow();
    d->m_currentClosestCurve = NULL;
    d->m_currentClosestPoint = -1;
    d->m_arrow->hide();
    d->formatter = nullptr;
}

void QuPlotMarkerComponent::attachToPlot(QuPlotBase *plot)
{
    d->m_arrow->attach(plot);
    d->m_marker->attach(plot);
}

void QuPlotMarkerComponent::connectToPlot(QuPlotBase *plot) {
    QObject::connect(d->m_picker, SIGNAL(selected(const QPolygon &)),
                     plot, SLOT(showMarker(const QPolygon &)));
}

/*
 * \brief Returns true if the marker has been updated, false otherwise
 *
 * A marker is updated when it's visible and the linked curve is visible and the text
 * hasn't changed
 */
bool QuPlotMarkerComponent::update(const QuPlotBase *plot)
{
    /* update arrow if the arrow is visible and if the curve has not been removed */
    if(d->m_arrow->isVisible() && plot->curves().contains(d->m_currentClosestCurve) &&
            d->m_currentClosestCurve->isVisible() &&
            d->m_currentClosestCurve && d->m_currentClosestPoint != -1)
    {
        double x = d->m_currentClosestCurve->data()->sample(d->m_currentClosestPoint).x();
        double y = d->m_currentClosestCurve->data()->sample(d->m_currentClosestPoint).y();
        d->m_arrow->begin = QPointF(x, y);
        //  d->m_arrow->end = QPointF(d->m_currentClosestCurve->x(closestPoint)+hoff, up);
        QwtText l = d->m_marker->label();
        QString mtxt = markerText(plot, d->m_currentClosestCurve, d->m_currentClosestPoint);
        if(mtxt != l.text()) {
            l.setText(mtxt);
            d->m_marker->setLabel(l);
            return true;
        }
    }
    return false;
}

QString QuPlotMarkerComponent::xLabel() const {
    return d->xText;
}

QString QuPlotMarkerComponent::yLabel() const {
    return d->yText;
}

/*! \brief returns the text currently displayed
 *
 * @return the marker label text
 */
QString QuPlotMarkerComponent::label() const
{
    return d->m_marker->label().text();
}

/*! \brief Use a custom implementation of QuPlotMarkerFormat_I to format the marker text
 *
 * This offers an alternative to the reimplementation of
 * QuPlotMarkerComponent::markerText in a subclass
 *
 * QuPlotMarkerFormat_I subclasses shall implement the QuPlotMarkerFormat_I::format
 * method to provide the marker's text.
 *
 * \note
 * Ownership of the passed object is QuPlotMarkerComponent's
 *
 * \note
 * Subsequent calls delete previously installed QuPlotMarkerFormat_I
 *
 * \note
 * To remove an installed formatter, call this method with a nullptr argument
 *
 * @param pmf an implementation of the QuPlotMarkerFormat_I interface
 *
 * @since 1.3.1
 */
void QuPlotMarkerComponent::setFormatter(QuPlotMarkerFormat_I *pmf) {
    if(d->formatter)
        delete d->formatter;
    d->formatter = pmf;
}

/*!
 * \brief return the installed QuPlotMarkerFormat_I formatter or null
 *
 * @see setFormatter
 */
QuPlotMarkerFormat_I *QuPlotMarkerComponent::formatter() const {
    return d->formatter;
}

/*! \brief set the marker label as QwtText
 *
 * @param text the marker label text
 */
void QuPlotMarkerComponent::setLabel(const QwtText &text)
{
    d->m_marker->setLabel(text);
}

void QuPlotMarkerComponent::update(const QuPlotBase *plot,
                                   QwtPlotCurve *closestC,
                                   int closestPointIdx)
{
    double hoff, range, markerW, markerH;
    double xlb, xub, ylb, yub;
    double y = plot->transform(closestC->yAxis(), closestC->data()->sample(closestPointIdx).y());
    double x = plot->transform(closestC->xAxis(), closestC->data()->sample(closestPointIdx).x());
    xlb = plot->transform(closestC->xAxis(), plot->axisScaleDiv(closestC->xAxis()).lowerBound());
    xub = plot->transform(closestC->xAxis(), plot->axisScaleDiv(closestC->xAxis()).upperBound());
    ylb = plot->transform(closestC->yAxis(), plot->axisScaleDiv(closestC->yAxis()).lowerBound());
    yub = plot->transform(closestC->yAxis(), plot->axisScaleDiv(closestC->yAxis()).upperBound());
    range = xub - xlb;

    d->m_currentClosestCurve = closestC;
    d->m_currentClosestPoint = closestPointIdx;

    QwtText l = d->m_marker->label();
    l.setText(markerText(plot, closestC, closestPointIdx));
    d->m_marker->setLabel(l);

    markerW = l.textSize(l.font()).width();
    markerH = l.textSize(l.font()).height();

    if(x <= xlb + range/3)
    {
        hoff = 0.1*range; // + .2*x;
        d->m_marker->setLabelAlignment(Qt::AlignRight|Qt::AlignBottom);
    }
    else if(x >= xlb + 2 * range/3)
    {
        hoff = -0.1*range;
        d->m_marker->setLabelAlignment(Qt::AlignLeft|Qt::AlignBottom);
    }
    else
    {
        hoff = -0.1*range;
        d->m_marker->setLabelAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    }

    QPointF begin, end, arrowEnd;
    double up = ylb + 0.93 * (yub - ylb);
    begin = QPointF(x, y);
    end = QPointF(x + hoff, up);

    d->m_marker->setYAxis(closestC->yAxis());
    d->m_marker->setXValue(plot->invTransform(closestC->xAxis(), end.x()));
    d->m_marker->setYValue(plot->invTransform(closestC->yAxis(), end.y()));

    int arrowXEndPix = x + hoff;
    int arrowYEndPix =  up;

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
    d->m_arrow->setYAxis(closestC->yAxis());
    d->m_arrow->begin = QPointF(plot->invTransform(closestC->xAxis(), begin.x()),
                                plot->invTransform(closestC->yAxis(), begin.y()));
    d->m_arrow->end = arrowEnd;
}

void QuPlotMarkerComponent::hide() {
    d->m_marker->hide();
    d->m_arrow->hide();
}

void QuPlotMarkerComponent::show() {
    d->m_marker->show();
    d->m_arrow->show();
}

bool QuPlotMarkerComponent::isVisible() const {
    return d->m_marker->isVisible();
}

QwtPlotCurve *QuPlotMarkerComponent::currentClosestCurve() const {
    return d->m_currentClosestCurve;
}

int QuPlotMarkerComponent::currentClosestPoint() const
{
    return d->m_currentClosestPoint;
}

Arrow *QuPlotMarkerComponent::getArrow() const {
    return d->m_arrow;
}

QwtPlotMarker *QuPlotMarkerComponent::qwtPlotMarker() const {
    return d->m_marker;
}

QwtPlotPicker *QuPlotMarkerComponent::qwtPlotPicker() const {
    return d->m_picker;
}

/*! \brief returns a string containing a textual representation of the point on the curve
 *         the marker refers to
 *
 * @param plot the pointer to the plot owning the marker
 * @param curve the curve to which the marker refers
 * @param index the index of the sample on the curve that the marker refers to
 *
 * This method can be reimplemented on a custom QuPlotMarkerComponent in order to
 * provide a text different from the default one.
 * An alternative to QuPlotMarkerComponent subclassing is offered by the
 * QuPlotMarkerFormat_I interface: just implement its format method and install
 * the object with setFormatter.
 *
 * \par Default text
 * The default text contains *x: [x value]\n[source name]:[source value]*
 *
 * \par Note
 * If you need further customization for the component, reimplement the
 * QuPlotMarkerComponent::update(const QuPlotBase *, QwtPlotCurve *, int )
 * method to provide a different placement strategy, text alignment and so on.
 *
 */
QString QuPlotMarkerComponent::markerText(const QuPlotBase *plot,
                                          const QwtPlotCurve *closest_curve,
                                          const int index)
{
    d->xText.clear();
    d->yText.clear();
    QString sx, sy;
    double y, x, y1;
    if(closest_curve != nullptr) {
        x = closest_curve->data()->sample(index).x();
        y = closest_curve->data()->sample(index).y();

        /* get overlapping curves in point x,y and add to the marker the y values */
        QList<QwtPlotCurve *> crossing_crvs = intersectingCurves(plot, x, y, closest_curve);
        /* add the current curve to the overlapping set */
        crossing_crvs.push_front((QwtPlotCurve *)(closest_curve)); /* remove constness */
        if(d->formatter)
            return d->formatter->format(plot, crossing_crvs, index, d->xText, d->yText);
        else {
            /* place the x coordinate label, taken from the x axis scale */
            const QwtScaleDraw *scaleDraw = plot->axisScaleDraw(closest_curve->xAxis());
            d->xText = QString("x: %1").arg(scaleDraw->label(x).text());
            sx = d->xText + "\n";

            foreach(QwtPlotCurve *pc, crossing_crvs) {
                sy.clear();
                if(pc->rtti() == QwtPlotItem::Rtti_PlotUserItem + RTTI_CURVE_OFFSET) {
                    QuPlotCurve *qpl = static_cast<QuPlotCurve *>(pc);
                    if(!qpl->text(x).isEmpty())
                        sy = QString("%1:\n%2").arg(pc->title().text()).arg(qpl->text(x));
                }
                if(sy.isEmpty()) {
                    y1 = pc->data()->sample(index).y();
                    /* place the curve title on the marker text first */
                    if(pc->title() != QwtText(QString()))
                        sy += QString("%1: ").arg(pc->title().text());
                    sy += QString("%1").arg(y1, 0, 'g', 5);
                }
                d->yText += sy + "\n";
                sx += sy + "\n";
            }
        }
    }
    return sx;
}

/*!
 * \brief QuPlotMarkerComponent::intersectingCurves
 * \param plot the plot
 * \param x x point to check for any curve crossing x and y
 * \param y y point to check for any curve crossing x and y
 * \param curve the closest curve, that will not be in the list
 * \return a list of curves that cross *curve* at (x,y)
 *
 * \note
 * The *curve* passed as input parameter is not included in the returned list
 */
QList<QwtPlotCurve *> QuPlotMarkerComponent::intersectingCurves(const QuPlotBase *plot,
                                                                const double x,
                                                                const double y,
                                                                const QwtPlotCurve* curve)
{
    QList<QwtPlotCurve *> ocurves;
    double vx, vy;
    foreach(QwtPlotCurve *c, plot->curves()) {
        /* curves not visible are not added to the overlapping set */
        if(c != curve && c->isVisible()) {
            for(size_t i = 0; i < c->dataSize(); i++)
            {
                vx = c->data()->sample(i).x();
                vy = c->data()->sample(i).y();
                if(x == vx && y == vy) {
                    ocurves.push_back(c);
                    break;
                }
            }
        }
    }
    return ocurves;
}
