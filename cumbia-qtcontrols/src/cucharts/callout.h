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

#ifndef CALLOUT_H
#define CALLOUT_H

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class CalloutPrivate;

class CalloutListener
{
public:
    virtual void onCalloutClicked(const QString& title, const QPointF& xy) = 0;
};

class Callout : public QGraphicsItem
{
public:
    Callout(QChart *parent, bool xDateTime, CalloutListener *cl);

    virtual ~Callout();

    void setText(const QString &title);
    void setAnchor(QPointF point,  bool move_marker = true);
    void updateGeometry();
    void show(const QString& title, const QPointF &pos, const QPointF& value, const QColor &color, bool move_marker = true);
    void setXDateTime(bool dt);
    bool xDateTime() const;
    QPointF anchor() const;
    QString title() const;
    bool isMoving() const;
    void setMoving(bool m);
    bool isPressed() const;
    void hide();
    void setVisible(bool v);
    bool isVisibleFor(const QString& title) const;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

private:


    CalloutPrivate *d;
};

#endif // CALLOUT_H
