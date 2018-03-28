#ifndef TLABELITEM_H
#define TLABELITEM_H

#include <QGraphicsSimpleTextItem>
#include <qtangocore.h>
#include <QObject>
#include <QVector>
#include <QRectF>
#include <EApplicationLauncher>
#include <com_proxy_reader.h>

class TLabelItem : public QObject, public QGraphicsSimpleTextItem, public QTangoComProxyReader
{
Q_OBJECT

    enum Mode { Simple, Zoomed };

public:
    TLabelItem(QObject * = 0);

    void setTrueColor(QColor c) { m_trueColor = c; };
    QColor trueColor() { return m_trueColor; };

    void setFalseColor(QColor c) { m_falseColor = c; };
    QColor falseColor() { return m_falseColor; };

    void setTrueString(QString s) { m_trueString = s; };
    QString trueString() { return m_trueString; };

    void setFalseString(QString s) { m_falseString = s; };
    QString falseString() { return m_falseString; };

//    QRectF boundingRect();

protected slots:
    void refresh(const TVariant& v);

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
//    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
//    void launchHelperApplication();

    QVector<QColor> 	m_stateColors;
    QColor		m_trueColor;
    QColor		m_falseColor;
    QString		m_trueString;
    QString		m_falseString;

    QColor		brushColor;
    Mode		m_mode;
    QRectF		ellipseRect;
//    EApplicationLauncher *app;
//    QString helperApplication;

};

#endif
