#include "quplotcanvaspaintercomponent.h"
#include <QPainter>
#include <QFont>
#include <cudata.h>
#include <qwt_plot_curve.h>
#include "quplot_base.h"

QuPlotCanvasPainterComponent::QuPlotCanvasPainterComponent()
{

}

QString QuPlotCanvasPainterComponent::name() const
{
    return "canvas_painter";
}

void QuPlotCanvasPainterComponent::update(QPainter *p,
                                          const QuPlotBase *plot,
                                          const QList<QwtPlotCurve *>curves,
                                          const CuData &options)
{
    QPen pen;
    QRectF myRect;
    QFont f = p->font();
    f.setPointSize(11.0);
    p->setFont(f);
    QFontMetrics fm(f);
    bool titleOnCanvasEnabled = options["show_title"].toBool();
    bool displayZoomHint = options["show_zoom_hint"].toBool();
    bool zoom_disabled = options["zoom_disabled"].toBool();
    if(titleOnCanvasEnabled)
    {
        int i = 0;
        foreach(QwtPlotCurve * c, curves)
        {
            if(c->isVisible())
            {
                myRect.setWidth(plot->width());
                myRect.setHeight(fm.height());
                myRect.setX(10);
                myRect.moveTop(20 + i * fm.height());
                // 	  printf("draw text x %f y %f w %f h %f\n", myRect.x(), myRect.y(), myRect.width(), myRect.height());
                p->setPen(c->pen());
                p->drawText(myRect, Qt::AlignLeft,  c->title().text());
                i++;
            }
        }
    }
    if(displayZoomHint && !zoom_disabled)
    {
        myRect.setSize(plot->geometry().size());
        myRect.setX(10);
        myRect.setY(plot->geometry().height() - plot->axisWidget(QwtPlot::xBottom)->height() - 40);
        QColor txtColor(Qt::darkGreen);
        txtColor.setAlpha(160);
        pen.setColor(txtColor);
        p->setPen(pen);
        p->drawText(myRect, Qt::AlignLeft,  "Press Shift Key to zoom");
    }
    /* no curves or curves without data? Inform the user */
    foreach(QwtPlotCurve * c, curves)
        if(c->dataSize() > 1)
            return;
    QString txt;
    curves.size() ? txt = "Waiting for data..." : txt = "No data";
    QFontMetrics fm2(f);
    p->setFont(f);
    myRect.setWidth(plot->width());
    myRect.setHeight(fm2.height());
    myRect.setX(10);
    myRect.moveTop(plot->canvas()->rect().height() -10 - fm2.height());
    QColor txtColor(Qt::darkGray);
    txtColor.setAlpha(160);
    pen.setColor(txtColor);
    p->setPen(pen);
    p->drawText(myRect, Qt::AlignLeft,  txt);
}
