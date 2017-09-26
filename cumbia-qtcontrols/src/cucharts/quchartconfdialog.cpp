#include "quchartconfdialog.h"
#include <cumacros.h>
#include <QtDebug>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>

QuChartConfDialog::QuChartConfDialog(QWidget *parent) : QDialog(parent)
{
    QGridLayout *lo = new QGridLayout(this);
    lo->setObjectName("mainLayout");
}

QuChartConfDialog::~QuChartConfDialog()
{
    pdelete("~CuChartConfDialog");
}

void QuChartConfDialog::applyClicked()
{
    QString curve = sender()->property("curve").toString();
    QGroupBox* gb = findChild<QGroupBox *>("gb_" + curve);
    QString xaxis, yaxis;
    xaxis = gb->findChild<QComboBox *>("cb_xaxis")->currentText();
    yaxis = gb->findChild<QComboBox *>("cb_yaxis")->currentText();
    emit curveAxisChanged(curve, xaxis);
    emit curveAxisChanged(curve, yaxis);


}

void QuChartConfDialog::addCurve(QAbstractSeries* s)
{
    QString xaxis, yaxis;
    QString name = s->objectName();
    QList<QAbstractAxis *> axes = s->attachedAxes();
    foreach(QAbstractAxis *a, axes)
        a->orientation() == Qt::Horizontal ? xaxis = name : yaxis = name;

    QGroupBox *gb = new QGroupBox(this);
    gb->setTitle("Curve " + name);
    gb->setObjectName("gb_" + name);
    QGridLayout *lo = new QGridLayout(gb);
    QLabel *label = new QLabel("Axes:", this);
    label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QComboBox *xac = new QComboBox(this);
    xac->setObjectName("cb_xaxis");
    xac->insertItems(0, QStringList() << "X1" << "X2");
    xac->setCurrentText(xaxis);
    QComboBox *yac = new QComboBox(this);
    yac->setObjectName("cb_yaxis");
    yac->insertItems(0, QStringList() << "Y1" << "Y2");
    yac->setCurrentText(yaxis);
    QPushButton *pb = new QPushButton("Apply", this);
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(applyClicked()));
    pb->setProperty("curve", name);
    lo->addWidget(label, 0, 0, 1, 1);
    lo->addWidget(xac, 0, 1, 1, 1);
    lo->addWidget(yac, 0, 2, 1, 1);
    lo->addWidget(pb, 0, 3, 1, 1);

    findChild<QGridLayout *>("mainLayout")->addWidget(gb, 0, 0, 1, 10);
}
