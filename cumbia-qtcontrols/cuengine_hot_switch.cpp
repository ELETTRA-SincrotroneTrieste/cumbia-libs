#include "cuengine_hot_switch.h"
#include <QObject>
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>

CuContext *CuEngineHotSwitch::hot_switch(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo)
{
    CuControlsReaderA *r = nullptr;
    CuControlsWriterA *w = nullptr;
    CuContext *c = nullptr;

    if(ctx && (r = ctx->getReader()) != nullptr) {
        const QString& src = r->source();
        if(!src.isEmpty()) {
            pretty_pri("disposing reader of old context %p", ctx);
            ctx->disposeReader(); // deletes r
            c = new CuContext(p, fpoo); // new context with p and fpoo
            pretty_pri("replace_reader src %s", qstoc(src));
            r = c->replace_reader(src.toStdString(), l); // same old source
            r->setSource(src);
        }
    }
    else if(ctx && (w = ctx->getWriter()) != nullptr) {
        const QString& t = w->target();
        if(!t.isEmpty()) {
            pretty_pri("disposing writer of old context %p", ctx);
            ctx->disposeWriter(); // deletes r
            c = new CuContext(p, fpoo); // new context with p and fpoo
            pretty_pri("replace_writer src %s", qstoc(t));
            w = c->replace_writer(t.toStdString(), l); // same old target
            w->setTarget(t);
        }
    }
    if(ctx)
        delete ctx;
    return c;
}

