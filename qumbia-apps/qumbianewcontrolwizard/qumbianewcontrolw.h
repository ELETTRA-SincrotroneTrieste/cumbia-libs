#ifndef QUMBIANEWCONTROLW_H
#define QUMBIANEWCONTROLW_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
};

#endif // QUMBIANEWCONTROLW_H
