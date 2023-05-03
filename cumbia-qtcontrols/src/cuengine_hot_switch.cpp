#include "cuengine_hot_switch.h"
#include <QObject>
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>


class CuEngineHotSwitchPrivate {
public:
    CuEngineHotSwitchPrivate() : error(false) {}

    const char m_engines[6][16] = { "no engine", "tango", "epics", "http", "random", "websocket" };
    bool error;
    QString msg;
};

CuEngineHotSwitch::CuEngineHotSwitch() {
    d = new CuEngineHotSwitchPrivate;
}

CuEngineHotSwitch::~CuEngineHotSwitch() {
    delete d;
}

/*!
 * \brief Perform the actual engine switch on either the reader or writer behind the given context. The old context
 *        will be deleted. The returned context shall be used by the caller to update its reference to its own context
 * \param l the data listener to which the new context with the new engine will be linked
 * \param ctx the current context of the reader (or writer) to be replaced
 * \param p a pointer to the CumbiaPool holding the *new engine configuration*
 * \param fpoo a const reference to the pool holding the *new engine configuration*
 * \return the new context holding a new reader (writer) belonging to the new engine
 *
 * This method is mainly used by the library components (or by custom cumbia components) when a context switch (i.e.
 * engine switch) is required by the application.
 *
 * Example: QuLabel::ctxSwitch(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
 *
 *
 */
CuContext *CuEngineHotSwitch::hot_switch(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo)
{
    CuControlsReaderA *r = nullptr;
    CuControlsWriterA *w = nullptr;
    CuContext *c = nullptr;
    d->error = false;
    d->error = (ctx == nullptr);
    if(d->error)
        d->msg = "given context is null";
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
    else if(ctx && !d->error && (w = ctx->getWriter()) != nullptr) {
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
    if(!d->error && w == nullptr && r == nullptr) {
        d->error = true;
        d->msg = "no reader nor writer in the given context";
    }
    if(ctx)
        delete ctx;
    return c;
}


/*!
 * \brief returns the engine name corresponding to the input value from the CuEngineHotSwitch::Engines enum
 * \param engine one value in the CuEngineHotSwitch::Engines enum
 * \return the name of the engine or nullptr if the input argument exceeds CuEngineHotSwitch::MaxEngines
 */
const char *CuEngineHotSwitch::engine_name(int engine) const {
    return engine < MaxEngines ? d->m_engines[engine] : nullptr;
}

