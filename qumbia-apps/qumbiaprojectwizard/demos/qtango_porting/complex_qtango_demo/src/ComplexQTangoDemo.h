#ifndef ComplexQTangoDemo_H
#define ComplexQTangoDemo_H

#include <QWidget>

namespace Ui {
class ComplexQTangoDemo;
}

class ComplexQTangoDemo : public QWidget
{
    Q_OBJECT

public:
    explicit ComplexQTangoDemo(QWidget *parent = 0);
    ~ComplexQTangoDemo();

private slots:
    void execDemoDialog();

private:
    void m_setup(); // install QTWatcher and QTWriter, set source/targets on custom reader and writer
    Ui::ComplexQTangoDemo *ui;
};

#endif // ComplexQTangoDemo_H
