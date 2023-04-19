#include "cumouse-ev-handler.h"
#include "cupluginloader.h"
#include "cumouseevhandlerplugin_i.h"
#include <cumacros.h>
#include <QRegularExpression>

class CuMouseEvHandler_P {
public:
    CuMouseEvHandler_P() : plugin(nullptr) {}
    QObject *plugin;
};

CuMouseEvHandler::CuMouseEvHandler() {
    d = new CuMouseEvHandler_P;
    if(!d->plugin) {
        CuPluginLoader cupl;
        QStringList pluginPaths = cupl.getPluginAbsoluteFilePaths(CUMBIA_QTCONTROLS_PLUGIN_DIR, QRegularExpression(".*mouse-event-plugin\\.so"));
        for(int i = 0; i < pluginPaths.size(); i++) {
            CuMouseEvHandlerPlugin_I *mousevhplugin = NULL;
            QPluginLoader pluginLoader(pluginPaths[i]);
            d->plugin = pluginLoader.instance();
            if (d->plugin){
                mousevhplugin = qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin);
            }
            if(!d->plugin || !mousevhplugin){
                perr("CuMouseEvHandler failed to load plugin \"%s\" under \"%s\"",
                     qstoc(pluginPaths[i]), CUMBIA_QTCONTROLS_PLUGIN_DIR);
            }
        } // for pluginPaths


    }
    else if(d->plugin && qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)) {
        printf("CuMouseEvHandler: plugin already loaded\n");
    }
}

CuMouseEvHandler::~CuMouseEvHandler() {

}

void CuMouseEvHandler::pressed(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->pressed(e, w, ctx);
}

void CuMouseEvHandler::released(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->released(e, w, ctx);
}

void CuMouseEvHandler::moved(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->moved(e, w, ctx);
}

void CuMouseEvHandler::doubleClicked(QMouseEvent *e, QWidget *w, CuContextI *ctx) {
    if(d->plugin)
        qobject_cast<CuMouseEvHandlerPlugin_I *> (d->plugin)->doubleClicked(e, w, ctx);
}
