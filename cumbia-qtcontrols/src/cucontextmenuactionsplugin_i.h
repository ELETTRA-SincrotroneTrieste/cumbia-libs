#ifndef CUCONTEXTMENUACTIONSPLUGIN_I_H
#define CUCONTEXTMENUACTIONSPLUGIN_I_H

#include <QList>
#include <QAction>

class CuContext;

class CuContextMenuActionsPlugin_I
{
public:
    virtual ~CuContextMenuActionsPlugin_I() {}

    virtual void setup(QWidget *widget, const CuContext* cuctx) = 0;

    virtual QList<QAction* >getActions() const = 0;
};

#define CuContextMenuActionsPlugin_I_iid "eu.elettra.qutils.CuContextMenuActionsPlugin_I"

Q_DECLARE_INTERFACE(CuContextMenuActionsPlugin_I, CuContextMenuActionsPlugin_I_iid)

#endif // CUCONTEXTMENUACTIONSPLUGIN_I_H
