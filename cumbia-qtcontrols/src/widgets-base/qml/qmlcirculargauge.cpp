#include "qmlcirculargauge.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQuickItem>
#include <QVBoxLayout>

QmlCircularGauge::QmlCircularGauge(QWidget *parent) : QWidget(parent)
{
  //  qmlRegisterType<QmlCircularGauge>("eu.elettra.cumbia.qtcontrols.qmlcirculargauge", 1, 0, "QmlCuircularGauge");
    QQuickView *qv = new QQuickView(QUrl("qrc:///qml/qmlcirculargauge.qml"));
    QWidget *widget = QWidget::createWindowContainer(qv, this);
    QVBoxLayout *lo = new QVBoxLayout(this);
    lo->addWidget(widget);
}
