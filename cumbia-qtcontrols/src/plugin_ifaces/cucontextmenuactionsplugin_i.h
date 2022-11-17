#ifndef CUCONTEXTMENUACTIONSPLUGIN_I_H
#define CUCONTEXTMENUACTIONSPLUGIN_I_H

#include <QList>
#include <QAction>

class CuContextI;

class CuContextMenuActionsPlugin_I
{
public:
    virtual ~CuContextMenuActionsPlugin_I() {}
    virtual int order() const = 0;
    virtual void setup(QWidget *widget, const CuContextI* cuctx_I) = 0;
    virtual QList<QAction* >getActions() const = 0;
};

#define CuContextMenuActionsPlugin_I_iid "eu.elettra.qutils.CuContextMenuActionsPlugin_I"

Q_DECLARE_INTERFACE(CuContextMenuActionsPlugin_I, CuContextMenuActionsPlugin_I_iid)

#endif // CUCONTEXTMENUACTIONSPLUGIN_I_H
