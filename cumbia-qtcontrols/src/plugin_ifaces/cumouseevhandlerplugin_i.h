#ifndef CUMOUSEEVHANDLERPLUGIN_I_H
#define CUMOUSEEVHANDLERPLUGIN_I_H

#include <QObject>
class QMouseEvent;
class CuContextI;

class CuMouseEvHandlerPlugin_I {
public:
    virtual void pressed(QMouseEvent *e, QWidget *w, CuContextI *ctx) = 0;
    virtual void released(QMouseEvent *e, QWidget *w, CuContextI *ctx) = 0;
    virtual void moved(QMouseEvent *e, QWidget *w, CuContextI *ctx) = 0;
    virtual void doubleClicked(QMouseEvent *e, QWidget *w, CuContextI *ctx) = 0;
};

#define CuMouseEvHandlerPlugin_I_iid "eu.elettra.qutils.CuMouseEvHandlerPlugin_I"

Q_DECLARE_INTERFACE(CuMouseEvHandlerPlugin_I, CuMouseEvHandlerPlugin_I_iid)

#endif // CUMOUSEEVHANDLERPLUGIN_I_H
