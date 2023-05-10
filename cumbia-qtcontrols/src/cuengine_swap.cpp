#include "cuengine_swap.h"
#include "cuengineaccessor.h"
#include "culog.h"
#include <QObject>
#include <cucontext.h>
#include <cumbia.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>
#include <cuserviceprovider.h>
#include <cuservicei.h>
#include <QMetaObject>
#include <QMetaProperty>

class CuEngineSwap_P {
public:
    CuEngineSwap_P() : error(false) {}
    bool error;
    QString msg;
};

CuEngineSwap::CuEngineSwap() {
    d = new CuEngineSwap_P;
}

CuEngineSwap::~CuEngineSwap() {
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
CuContext *CuEngineSwap::replace(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo)
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
            ctx->disposeReader(); // deletes r
            c = new CuContext(p, fpoo); // new context with p and fpoo
            r = c->replace_reader(src.toStdString(), l); // same old source
            r->setSource(src);
        }
    }
    else if(ctx && !d->error && (w = ctx->getWriter()) != nullptr) {
        const QString& t = w->target();
        if(!t.isEmpty()) {
            ctx->disposeWriter(); // deletes r
            c = new CuContext(p, fpoo); // new context with p and fpoo
            w = c->replace_writer(t.toStdString(), l); // same old target
            w->setTarget(t);
        }
    }
    if(!d->error && w == nullptr && r == nullptr) {
        d->error = true;
        d->msg = "no reader nor writer in the given context";
    }
    if(d->error) {
        perr("CuEngineSwap.replace: %s", qstoc(d->msg));
        c = ctx;
    }
    else
        delete ctx;
    return c;
}

bool CuEngineSwap::check_objs_have_ctx_swap(const QObject *root, QString *oclass, QString *onam) const {
    bool ok = true;
    QList<QObject *> objs = root->findChildren<QObject *>();
    for(int i = 0; i < objs.size() && ok; i++) {
        const QObject *o = objs[i];
        if(o->metaObject()->indexOfProperty("source") > -1 || o->metaObject()->indexOfProperty("target") > -1) {
            ok = (o->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("ctxSwap(CumbiaPool *, const CuControlsFactoryPool &)")) >= 0);
            printf("seeing if object %s class %s has ctxSwap...index %d\n",
                   qstoc(o->objectName()), o->metaObject()->className(), o->metaObject()->indexOfSlot("ctxSwap"));

            if(!ok) {
                *oclass = o->metaObject()->className();
                *onam = o->objectName();
                printf("%s [%s]: properties\n", qstoc(o->objectName()), o->metaObject()->className());
                for(i = 0; i < o->metaObject()->propertyCount() ; i++)
                    printf("%d: %s\n", i, o->metaObject()->property(i).name());

                printf("\e[1;34m%s [%s]: methods\n", qstoc(o->objectName()), o->metaObject()->className());
                for(i = 0; i < o->metaObject()->methodCount() ; i++)
                    printf("%d: %s\n", i, o->metaObject()->method(i).name().data());
                printf("\e[0m\n");
            }
        }
    }
    return ok;
}

bool CuEngineSwap::check_root_has_engine_accessor(const QObject *root) const {
    return root->findChild<CuEngineAccessor *>() != nullptr;
}

/*! \brief swap engine on all children of root having either a source or target property and implementing
 *         the ctxSwap *qt slot*
 *
 * @param root the parent object used to find children having either a source or target property
 * @param cu_pool a pointer to the CumbiaPool with the target engine
 * @param m_ctrl_factory_pool a const reference to a factory pool with the target engine configured
 *
 * @return the number of objects whose engine has been successfully replaced
 *
 * You may want to check for error and msg for diagnostic purposes
 */
int CuEngineSwap::ctx_swap(const QObject *root, CumbiaPool *cu_pool, const CuControlsFactoryPool &m_ctrl_factory_pool) {
    d->msg.clear();
    bool ok;
    int i = 0;
    QList<QObject *> objs = root->findChildren<QObject *>();
    foreach(QObject *o, objs) {
        i++;
        const QString& ona = o->objectName();
        const char *clna = o->metaObject()->className();
        if(o->metaObject()->indexOfProperty("source") > 0 || o->metaObject()->indexOfProperty("target") > -1) {
            printf("%d. \e[1;32mobject %s type %s\e[0m\n", i, qstoc(o->objectName()), o->metaObject()->className());
            printf(" source: '%s' target '%s' \e[1;35mchild of %s \e[0;35m(%s)\e[0m\n", qstoc(o->property("source").toString()), qstoc(o->property("target").toString()),
                   o->parent() != nullptr ? qstoc(o->parent()->objectName()) : "(null)",
                   o->parent() != nullptr ? o->parent()->metaObject()->className() : "-");
            d->error = !QMetaObject::invokeMethod(o, "ctxSwap", Q_RETURN_ARG(bool, ok), Q_ARG(CumbiaPool*, cu_pool), Q_ARG(CuControlsFactoryPool, m_ctrl_factory_pool));
            if(d->error) d->msg += QString("error invoking method ctxSwap on object \"%1\" type \"%2\"\n") .arg(ona).arg(clna);
            else if(!ok) d->msg += QString("method ctxSwap on object \"%1\" type \"%2\" returned an error").arg(ona).arg(clna);
        }
    }
    return i;
}

bool CuEngineSwap::app_engine_swap(const QObject *root, CumbiaPool *cu_pool, const CuControlsFactoryPool &fp) {
    CuEngineAccessor *ea = root->findChild<CuEngineAccessor *>();
    d->error = !ea;
    if(!ea) {
        d->error = true;
        d->msg = QString("object %1 class %2 does not have an engine accessor").arg(root->objectName()).arg(root->metaObject()->className());
    } else {
        ea->engine_swap(cu_pool, fp);
    }
    return !d->error;
}

int CuEngineSwap::engine_type(const CuContext *ctx) const {
    // get current engine in use
    Cumbia *c = nullptr;
    d->error = (ctx == nullptr);
    if(!d->error) {
        pretty_pri("readers size from ctx %p: %d", ctx, ctx->readers().size());
        if(ctx->readers().size() > 0)
            c = m_cumbia_get_from_r(ctx);
        else if(ctx->writers().size() > 0)
            c = m_cumbia_get_from_w(ctx);
        else {
            d->error = true;
            d->msg = "no reader nor writer found in context";
        }
    }
    if(!d->error && !c) {
        d->error = true;
        d->msg = QString("no cumbia found in the given %1").arg(ctx->getReader() ? "reader" : "writer");
    }
    return  c == nullptr ? -1 : c->getType();
}

bool CuEngineSwap::same_engine(CumbiaPool *cu_pool, int engine) const {
    bool err = false;
    for(size_t i = 0; i < cu_pool->names().size() && !err; i++) {
        const std::string n = cu_pool->names().at(i);
        printf("\e[1;35m same_engine: seeing if %s matches  target engine %d \n", n.c_str(), engine);
        if(cu_pool->get(n))
            err = (cu_pool->get(n)->getType() == engine);
    }
    return err;
}

bool CuEngineSwap::log_move(const std::vector<Cumbia *>& vc, CumbiaPool *new_p) const {
    Cumbia *c = nullptr;
    if(vc.size() == 1 && vc[0]->getServiceProvider()) {
        CuLog * log = static_cast<CuLog *>(vc[0]->getServiceProvider()->get(CuServices::Log));
        if(log) {
            for(const std::string& n : new_p->names()) {
                c = new_p->get(n);
                if(c) c->getServiceProvider()->registerSharedService(CuServices::Log, log);
            }
        }
    }
    return c != nullptr;
}

int CuEngineSwap::cumbias_delete(const std::vector<Cumbia *> &vc) {
    size_t i = 0;
    for(size_t i = 0; i < vc.size(); i++) {
        vc[i]->getServiceProvider()->unregisterService(CuServices::Log); // uninstall
        pretty_pri("deleting \e[1;31mcumbia %p: type %d \e[0m", vc[i], vc[i]->getType());
        delete vc[i];
    }
    return i;
}

std::vector<Cumbia *> CuEngineSwap::cumbia_clear(CumbiaPool *cu_pool, CuControlsFactoryPool &fp) {
    d->msg.clear();
    d->error = false;
    std::vector<Cumbia*> vc;
    // clear the pool
    for(size_t i = 0; i < cu_pool->names().size() && !d->error; i++) {
        const std::string n = cu_pool->names().at(i);
        d->error = !cu_pool->get(n);
        if(d->error)
            d->msg = QString("error getting cumbia instance for \"%1\"").arg(n.c_str());
    }
    if(!d->error) {
        for(const std::string& n : cu_pool->names()) {
            printf("unregistering cumbia impl '%s'\n", n.c_str());
            vc.push_back(cu_pool->get(n));
            cu_pool->unregisterCumbiaImpl(n);
            cu_pool->clearSrcPatterns(n);
        }
        fp = CuControlsFactoryPool();
    }
    pretty_pri("m_clear_cumbia: error? %s msg %s cumbias found %ld", d->error ? "YES" : "NO", qstoc(d->msg),
               vc.size());
    return vc;
}

Cumbia *CuEngineSwap::m_cumbia_get_from_r(const CuContext *ctx) const {
    Cumbia *c = nullptr;
    const int t0 = ctx->readers().at(0)->getCumbia()->getType();
    pretty_pri("getting cumbia from %d readers", ctx->readers().size());
    int i = 0, t{t0}; // all readers must have the same cumbia type
    for(i = 1; i < ctx->readers().size() && t == t0; i++)
        t = ctx->readers().at(i)->getCumbia()->getType();
    d->error = (i < ctx->readers().size());
    if(d->error)
        d->msg = "multiple readers with mixed cumbia types";
    else
        c = ctx->readers().at(0)->getCumbia();
    return c;
}

Cumbia *CuEngineSwap::m_cumbia_get_from_w(const CuContext *ctx) const {
    Cumbia *c = nullptr;
    const int t0 = ctx->writers().at(0)->getCumbia()->getType();
    int i = 0, t{t0}; // all writers must have the same cumbia type
    for(i = 1; i < ctx->writers().size() && t == t0; i++)
        t = ctx->writers().at(i)->getCumbia()->getType();
    d->error = (i < ctx->writers().size());
    if(d->error)
        d->msg = "multiple writers with mixed cumbia types";
    else
        c = ctx->writers().at(0)->getCumbia();
    return c;
}

bool CuEngineSwap::ok() const {
    return !d->error;
}

bool CuEngineSwap::error() const {
    return d->error;
}

const QString &CuEngineSwap::msg() const {
    return d->msg;
}
