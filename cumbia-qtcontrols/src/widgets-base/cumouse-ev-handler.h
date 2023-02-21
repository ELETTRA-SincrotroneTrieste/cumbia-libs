#ifndef CUMOUSE_EV_HANDLER_H
#define CUMOUSE_EV_HANDLER_H


class QWidget;
class QMouseEvent;
class CuMouseEvHandler_P;
class CuContextI;

class CuMouseEvHandler {
public:
    CuMouseEvHandler();
    ~CuMouseEvHandler();
    void pressed(QMouseEvent *e, QWidget *w, CuContextI *ctx);
    void released(QMouseEvent *e, QWidget *w, CuContextI *ctx);
    void moved(QMouseEvent *e, QWidget *w, CuContextI *ctx);
    void doubleClicked(QMouseEvent *e, QWidget *w, CuContextI *ctx);
private:
    CuMouseEvHandler_P *d;
};

#endif // CUMOUSE-EV-HANDLER_H
