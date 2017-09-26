#ifndef QUCHARTCONFDIALOG_H
#define QUCHARTCONFDIALOG_H

#include <QDialog>
#include <QtCharts/QAbstractSeries>

using namespace QtCharts;

class QuChartConfDialog : public QDialog
{
    Q_OBJECT
public:
    QuChartConfDialog(QWidget *parent);

    virtual ~QuChartConfDialog();

    void addCurve(QAbstractSeries *s);

public slots:
    void applyClicked();

signals:
    void curveAxisChanged(const QString& curve, const QString& axname);
};

#endif // CUCHARTCONFDIALOG_H
