#ifndef Bpmqtango_H
#define Bpmqtango_H

#include <QWidget>

namespace Ui {
class Bpmqtango;
}

class Bpmqtango : public QWidget
{
    Q_OBJECT

public:
    explicit Bpmqtango(QWidget *parent = 0);
    ~Bpmqtango();

private:
    Ui::Bpmqtango *ui;
};

#endif // Bpmqtango_H
