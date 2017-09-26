/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "callout.h"
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QMouseEvent>
#include <QtCharts/QChart>
#include <QDateTime>
#include <QtDebug>

class CalloutPrivate
{
public:
    QString m_title, m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
    QFont m_font;
    QChart *m_chart;
    bool m_xDateTime;
    QColor m_bgColor;
    int m_colorAlpha;
    bool m_moving, m_pressed;
    CalloutListener *m_listener;
};

Callout::Callout(QChart *chart, bool xDateTime, CalloutListener *cl):
    QGraphicsItem(chart)

{
    d = new CalloutPrivate;
    setZValue(1000);
    setOpacity(0.4);
    d->m_chart = chart;
    d->m_moving = false;
    d->m_xDateTime = xDateTime;
    d->m_colorAlpha = 180;
    d->m_listener = cl;
    d->m_pressed = false;
    hide();
}

Callout::~Callout()
{
    delete d;
}

QRectF Callout::boundingRect() const
{
    QPointF anchor = mapFromParent(d->m_chart->mapToPosition(d->m_anchor));
    QRectF rect;
    rect.setLeft(qMin(d->m_rect.left(), anchor.x()));
    rect.setRight(qMax(d->m_rect.right(), anchor.x()));
    rect.setTop(qMin(d->m_rect.top(), anchor.y()));
    rect.setBottom(qMax(d->m_rect.bottom(), anchor.y()));
    return rect;
}

void Callout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPainterPath path;
    path.addRoundedRect(d->m_rect, 5, 5);

    QPointF anchor = mapFromParent(d->m_chart->mapToPosition(d->m_anchor));
    if (!d->m_rect.contains(anchor)) {
        QPointF point1, point2;

        // establish the position of the anchor point in relation to d->m_rect
        bool above = anchor.y() <= d->m_rect.top();
        bool aboveCenter = anchor.y() > d->m_rect.top() && anchor.y() <= d->m_rect.center().y();
        bool belowCenter = anchor.y() > d->m_rect.center().y() && anchor.y() <= d->m_rect.bottom();
        bool below = anchor.y() > d->m_rect.bottom();

        bool onLeft = anchor.x() <= d->m_rect.left();
        bool leftOfCenter = anchor.x() > d->m_rect.left() && anchor.x() <= d->m_rect.center().x();
        bool rightOfCenter = anchor.x() > d->m_rect.center().x() && anchor.x() <= d->m_rect.right();
        bool onRight = anchor.x() > d->m_rect.right();

        // get the nearest d->m_rect corner.
        qreal x = (onRight + rightOfCenter) * d->m_rect.width();
        qreal y = (below + belowCenter) * d->m_rect.height();
        bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
        bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

        qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
        qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);;
        point1.setX(x1);
        point1.setY(y1);

        qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);;
        qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10);;
        point2.setX(x2);
        point2.setY(y2);

        path.moveTo(point1);
        path.lineTo(anchor);
        path.lineTo(point2);
        path = path.simplified();
    }
    painter->setBrush(QColor(255, 255, 255));
    painter->drawPath(path);
    d->m_bgColor.setAlpha(d->m_colorAlpha);
    painter->fillPath(path, d->m_bgColor);
    painter->drawText(d->m_textRect, d->m_text);
}

void Callout::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(true);
}

void Callout::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
   //     setMoving(true);
        event->setAccepted(true);
    } else {
        event->setAccepted(false);
    }
}

void Callout::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    qDebug () << __FUNCTION__ << "RECONCHA DE LA LORA";
    if(d->m_listener)
    {
        qDebug () << __FUNCTION__ << "RECONCHA DE LA LORERRIMAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
        d->m_listener->onCalloutClicked(d->m_title, d->m_anchor);
        e->setAccepted(true);
    }
}

void Callout::setText(const QString &text)
{
    d->m_text = text;
    QFontMetrics metrics(d->m_font);
    d->m_textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, text);
    d->m_textRect.translate(5, 5);
    prepareGeometryChange();
    d->m_rect = d->m_textRect.adjusted(-5, -5, 5, 5);
}

void Callout::setAnchor(QPointF point, bool move_marker)
{
    d->m_anchor = point;
    if(move_marker && pos() != d->m_chart->mapToPosition(d->m_anchor) + QPoint(10, -50))
        setPos(d->m_chart->mapToPosition(d->m_anchor) + QPoint(10, -50));
}

void Callout::updateGeometry()
{
    prepareGeometryChange();
    setPos(d->m_chart->mapToPosition(d->m_anchor) + QPoint(10, -50));
}

/** \brief Place and show a marker callout on the plot.
 *
 * @param title: the title
 * @param p the position in plot coordinates, x,y
 * @param color the background of the callout
 * @param move_marker true: move the marker close to the point
 * @param move_marker false: keep the marker in the same position, move only the arrow
 */
void Callout::show(const QString &title, const QPointF &p, const QPointF &value, const QColor& color, bool move_marker)
{
    d->m_bgColor = color;
    d->m_title = title;
    if(d->m_xDateTime)
        setText(QString("%1\nX: %2 \nY: %3 ").arg(title).arg(QDateTime::fromMSecsSinceEpoch(value.x()).toString()).arg(value.y()));
    else
        setText(QString("%1\nX: %2 \nY: %3 "). arg(title).arg(value.x()).arg(value.y()));

    setAnchor(p, move_marker);

    if(!isVisible())
        QGraphicsItem::show();
}

void Callout::setXDateTime(bool dt)
{
    d->m_xDateTime = dt;
}

bool Callout::xDateTime() const
{
    return d->m_xDateTime;
}

QPointF Callout::anchor() const
{
    return d->m_anchor;
}

QString Callout::title() const
{
    return d->m_title;
}

bool Callout::isMoving() const
{
    return d->m_moving;
}

void Callout::setMoving(bool m)
{
    d->m_moving = m;
    m ? d->m_colorAlpha = 230 : d->m_colorAlpha = 160;
    update();
}

bool Callout::isPressed() const
{
    return d->m_pressed;
}

void Callout::hide()
{
    QGraphicsItem::hide();
    d->m_pressed = d->m_moving = false;
}

void Callout::setVisible(bool v)
{
    d->m_pressed = d->m_moving = false;
    QGraphicsItem::setVisible(v);
}

bool Callout::isVisibleFor(const QString &title) const
{
    return (this->title() == title && this->isVisible());
}
