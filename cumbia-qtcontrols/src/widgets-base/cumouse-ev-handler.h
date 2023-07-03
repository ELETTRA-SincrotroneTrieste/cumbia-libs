#ifndef CUMOUSE_EV_HANDLER_H
#define CUMOUSE_EV_HANDLER_H


class QWidget;
class QMouseEvent;
class CuMouseEvHandler_P;
class CuContextI;

/*!
 * \brief mouse event handler for cumbia qtcontrols widgets
 *
 * The CuMouseEvHandler instantiates a CuMouseEvHandlerPlugin_I plugin instance on demand.
 * No plugin is loaded unless any of the pressed, released, moved or doubleClicked methods
 * are called.
 */
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
