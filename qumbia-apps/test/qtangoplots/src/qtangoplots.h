#ifndef QTangoPlots_H
#define QTangoPlots_H

#include <QWidget>

namespace Ui {
class QTangoPlots;
}

class QTangoPlots : public QWidget
{
    Q_OBJECT

public:
    explicit QTangoPlots(QWidget *parent = 0);
    ~QTangoPlots();

private slots:
    void updateAll();

private:
    Ui::QTangoPlots *ui;

    int m_nPoints;
    int m_bufsiz;
};

#endif // QTangoPlots_H
