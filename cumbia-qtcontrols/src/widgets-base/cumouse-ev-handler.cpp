#include "cumouse-ev-handler.h"
#include "cupluginloader.h"
#include "cumouseevhandlerplugin_i.h"
#include <cumacros.h>
#include <QRegularExpression>

class CuMouseEvHandler_P {
public:
    CuMouseEvHandler_P() {
        CuPluginLoader cupl;
        QStringList pluginPaths = cupl.getPluginAbsoluteFilePaths(CUMBIA_QTCONTROLS_PLUGIN_DIR, QRegularExpression(".*mouse-event-plugin\\.so"));
        for(int i = 0; i < pluginPaths.size(); i++) {
            CuMouseEvHandlerPlugin_I *mousevhplugin = NULL;
            QPluginLoader pluginLoader(pluginPaths[i]);
            plugin = pluginLoader.instance();
            if (plugin){
                mousevhplugin = qobject_cast<CuMouseEvHandlerPlugin_I *> (plugin);
            }
            if(!plugin || !mousevhplugin){
                perr("CuMouseEvHandler failed to load plugin \"%s\" under \"%s\"",
                     qstoc(pluginPaths[i]), CUMBIA_QTCONTROLS_PLUGIN_DIR);
            }
        } // for pluginPaths


    }
    QObject *plugin;
};

// nothing is loded unless an event takes place
CuMouseEvHandler::CuMouseEvHandler() : d(nullptr) {
}

CuMouseEvHandler::~CuMouseEvHandler() {
    if(d) delete d;
}

void CuMouseEvHandler::pressed(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(!d) // alloc plugin on demand
        d = new CuMouseEvHandler_P;
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->pressed(e, w, ctx);
}

void CuMouseEvHandler::released(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(!d)
        d = new CuMouseEvHandler_P;
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->released(e, w, ctx);
}

void CuMouseEvHandler::moved(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(!d)
        d = new CuMouseEvHandler_P;
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->moved(e, w, ctx);
}

void CuMouseEvHandler::doubleClicked(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(!d)
        d = new CuMouseEvHandler_P;
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->doubleClicked(e, w, ctx);
}
