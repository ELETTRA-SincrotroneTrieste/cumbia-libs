#ifndef QUAPPS_H
#define QUAPPS_H

#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <culog.h>
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>

#include <QStringList>

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
#include <cuepregisterengine.h>
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cutangoregisterengine.h>
#endif
#ifdef CUMBIA_RANDOM_VERSION
#include <curndregisterengine.h>
#endif
#ifdef CUMBIA_WEBSOCKET_VERSION
#include <cuwsregisterengine.h>
#endif
#ifdef CUMBIA_HTTP_VERSION
#include <cuhttpregisterengine.h>
#endif

class CuModuleLoaderPrivate {
public:
    CuModuleLoaderPrivate() : log(nullptr), error(false) {}

    CuLog* log;
    QStringList modules;
    bool error;
    QString msg;
};

/*!
 * \brief Find and load installed cumbia modules.
 *
 * \par Foreword
 * In this documentation, the words *module, engine, domain* are synonyms.
 *
 * CuModuleLoader detects the installed cumbia modules, such as *cumbia-tango, cumbia-epics,
 * cumbia-websocket, cumbia-http, cumbia-random*. In order to be found, each module must have
 * been compiled and installed either through the scripts/cubiuild.sh automatic procedure or
 * manually.
 *
 * To get the list of searched modules for the current cumbia version, inspect *quapps.pri*.
 *
 * \par Usage
 * The application Qt project file (*.pro*) shall include *$${CUMBIA_ROOT}/include/quapps/quapps.pri*
 * instead of an engine specific *.pri* project include file, for examaple *qumbia-tango-controls.pri*.
 *
 * Including *quapps.pri* in your project *.pro* file and using CuModuleLoader
 * allows engine auto detection and loading.
 *
 * \par Loading precedence.
 *
 *
 * \li *cumbia-websocket*: search a *websocket url option*, <em>-u ws://websocket.url</em> or
 *    <em>-u  wss://websocket.secure.url</em>.
 * \li *cumbia-http*: search a *http url option*, <em>-u http://my.http.url/channel_name</em>, or the
 *     *https* counterpart. Note the *channel_name* that must be specified after the server url,
 *     where Server Sent Events are published.
 *
 * *cumbia-websocket* and *cumbia-http* are *mutually exclusive*. If either is loaded, *module search
 * stops*. This lets the same application work seamlessly across different engines without changing
 * its code.
 *
 * \li all remaining modules can *coexist*, for example both *EPICS* and *Tango* modules can be
 *     loaded allowing apps to connect to the engines natively. They can coexist because different
 *     engine sources are expected to have different forms.
 *
 */
class CuModuleLoader
{
private:
    CuModuleLoaderPrivate *d;

public:
    CuModuleLoader() {
        d = new CuModuleLoaderPrivate;
    }

    /*!
     * \brief Class constructor. Detects available modules and loads them according to command line
     *        options.
     *
     * \param cu_pool pointer to a valid CumbiaPool. Ownership is *not* taken.
     * \param ctrl_f_pool pointer to a valid CuControlsFactoryPool. Ownership is *not* taken
     * \param log_impl pointer to a valid CuLogImplI implementation. Ownership is *not* taken.
     *
     * Modules listed in *quapps.pri* are searched. If found, they are loaded.
     *
     * \par Http and websocket modules
     * If the *-u* command line argument is followed by an *URL*, either a *http/https* or a
     * *ws/wss* URL is expected. In the former case, the URL of the server must be followed by
     * a *channel name*, used for the *publisher/subscriber* communication.
     *
     * If *-u + url* is detected, one of *cumbia-http* or *cumbia-websocket* modules is loaded.
     * Since a cumbia application is studied to rely on any supported engine at runtime transparently,
     * no other modules are loaded after either *http* or *websocket*. They indeed share the same
     * source syntax as the native engines.
     *
     * If no URL is provided, all the other available engines are loaded.
     *
     * Parameters *cu_pool* and *ctrl_f_pool* must be valid.
     * If *log_impl* is null, then no CuLog service is instantiated and registered.
     * If *log_impl* is not null, a CuLog is created for you with the given CuLogImplI
     * instance, and can be fetched with the CuModuleLoader::log method.
     * If a CuLog is instantiated, it will be deleted together with CuModuleLoader.
     *
     * The CuModuleLoader::modules method returns the list of modules (or, equivalently
     * *engines, domains*) loaded.
     *
     */
    CuModuleLoader(CumbiaPool *cu_pool, CuControlsFactoryPool *ctrl_f_pool, CuLogImplI* log_impl) {
        d = new CuModuleLoaderPrivate;
        log_impl ? d->log = new CuLog(log_impl) : d->log = nullptr;

        // websocket engine
#ifdef CUMBIA_WEBSOCKET_VERSION
        Cumbia *cuws = nullptr;
        CuWsRegisterEngine wsre;
        if(wsre.hasCmdOption(qApp->arguments())) {
            cuws = wsre.registerWithDefaults(cu_pool, *ctrl_f_pool);
            static_cast<CumbiaWebSocket *>(cuws)->openSocket();
            cuws->getServiceProvider()->registerSharedService(CuServices::Log, d->log);
            d->modules << "websocket";
        }
#endif

#ifdef CUMBIA_HTTP_VERSION
        Cumbia *cuhttp = nullptr;
        if(!d->modules.contains("websocket")) {
            CuHttpRegisterEngine httpre;
            if(httpre.load(qApp->arguments())) {
                cuhttp = httpre.registerWithDefaults(cu_pool, *ctrl_f_pool);
                cuhttp->getServiceProvider()->registerSharedService(CuServices::Log, d->log);
                d->modules << "http";
            }
        }
#endif
        // other engines, if both websocket and http are not in use
        if(!d->modules.contains("websocket") && !d->modules.contains("http")) {
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
            Cumbia *cuta = nullptr;
            CuTangoRegisterEngine tare;
            cuta = tare.registerWithDefaults(cu_pool, *ctrl_f_pool);
            cuta->getServiceProvider()->registerSharedService(CuServices::Log, d->log);
            d->modules << "tango";
#endif
#ifdef CUMBIA_RANDOM_VERSION
            Cumbia *cura = nullptr;
            CuRndRegisterEngine rndre;
            cura = rndre.registerWithDefaults(cu_pool, *ctrl_f_pool);
            cura->getServiceProvider()->registerSharedService(CuServices::Log, d->log);
            d->modules << "random";
#endif
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
            Cumbia *cuep = nullptr;
            CuEpRegisterEngine epre;
            cuep = epre.registerWithDefaults(cu_pool, *ctrl_f_pool);
            cuep->getServiceProvider()->registerSharedService(CuServices::Log, d->log);
            d->modules << "epics";
#endif
        }
    }

    virtual ~CuModuleLoader() {
        //        if(d->log) delete d->log;
        delete d;
    }

    CuLog *log() const {
        return d->log;
    }

    QStringList modules() const {
        return d->modules;
    }

    QString msg() const {
        return d->msg;
    }

    bool error() const {
        return d->error;
    }

    bool switch_engine(int engine, CumbiaPool *cu_pool, CuControlsFactoryPool &m_ctrl_factory_pool, QList<QObject *>&objs) {
        std::vector<Cumbia *> cumbias;
        bool ok = m_prepare_switch(engine, cu_pool, m_ctrl_factory_pool, cumbias);
        if(ok) {
            ok = m_apply_switch(objs, cu_pool, m_ctrl_factory_pool) > 0;
            if(!ok) {
                d->error = true;
                d->msg = "no suitable objects found";
            }
        }
        if(ok)
            m_delete_cumbias(cumbias);
         return ok;
    }

    bool switch_engine(int engine, CumbiaPool *cu_pool, CuControlsFactoryPool &m_ctrl_factory_pool, QObject *root) {
        std::vector<Cumbia *> cumbias;
        bool ok = m_prepare_switch(engine, cu_pool, m_ctrl_factory_pool, cumbias);
        if(ok) {
            ok = m_apply_switch(root->findChildren<QObject *>(), cu_pool, m_ctrl_factory_pool) > 0;
            if(!ok) {
                d->error = true;
                d->msg = "no suitable objects found";
            }
        }
        if(ok)
            m_delete_cumbias(cumbias);
         return ok;
    }

    bool m_prepare_switch(int engine, CumbiaPool *cu_pool, CuControlsFactoryPool &m_ctrl_factory_pool, std::vector<Cumbia *>& cumbias) {
#if !defined(CUMBIA_HTTP_VERSION) && !defined(QUMBIA_TANGO_CONTROLS_VERSION)
        d->error = true;
        d->msg = "neither Tango nor Http modules are compiled into the library";
#else
        d->error = false;
#endif
        // supported engines: Tango and Http
        d->error = (engine != CumbiaHttp::CumbiaHTTPType && engine != CumbiaTango::CumbiaTangoType);
        if(d->error)
            d->msg = "engine hot switch supports the Tango and Http engines only";
        else {
            d->error = same_engine(cu_pool, engine);
            if(d->error) {
                d->msg = "trying to switch to the same engine";
            }
            else { // switch to the new engine
                // save the log instance
                // when engine switching, we suppose there is only one Cumbia in the pool
                // because interchangeable engines are mutually exclusive (see cclear.size() == 1 below)
                //
                CuLog *log = nullptr;
                Cumbia *cu = nullptr; // new cumbia
                if(engine == CumbiaTango::CumbiaTangoType) {
                    CuTangoRegisterEngine tare;
                    cumbias = m_clear_cumbia(cu_pool, m_ctrl_factory_pool);
                    if(cumbias.size() == 1 && cumbias[0]->getServiceProvider()) // recycle log
                        log = static_cast<CuLog *>(cumbias[0]->getServiceProvider()->get(CuServices::Log));
                    cu = tare.registerWithDefaults(cu_pool,  m_ctrl_factory_pool);
                }
                else if(engine == CumbiaHttp::CumbiaHTTPType) {
                    CuHttpRegisterEngine httpre;
                    d->error = !httpre.load(qApp->arguments(), true); // true: try loading also without -u in args
                    if(!d->error) {
                        cumbias = m_clear_cumbia(cu_pool, m_ctrl_factory_pool);
                        if(cumbias.size() == 1 && cumbias[0]->getServiceProvider()) // recycle log
                            log = static_cast<CuLog *>(cumbias[0]->getServiceProvider()->get(CuServices::Log));
                        cu = httpre.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
                    }
                    else
                        d->msg = "error registering http module: valid URL '" + httpre.url() + "' ?";
                }
                for(const std::string &n : cu_pool->names()) {
                    printf("+ cumbia registered %s -> %p type %d\n", n.c_str(), cu_pool->get(n), cu_pool->get(n)->getType());
                }

                // install log instance saved from earlier cumbia
                if(cu && log)
                    cu->getServiceProvider()->registerSharedService(CuServices::Log, log);
                else
                    pretty_pri("\033[1;33m*\033[0m: no log service installed in new cumbia: no log service found in older instance");
            } // !d->error trying to switch to the same engine
        }  // ! error unsupported target engine
        return !d->error;
    }

    int m_apply_switch(const QList<QObject *> objs, CumbiaPool *cu_pool, const CuControlsFactoryPool &m_ctrl_factory_pool) {
        int i = 0;
        foreach(QObject *o, objs) {
            i++;
            if(o->metaObject()->indexOfProperty("source") > 0 || o->metaObject()->indexOfProperty("target") > -1) {
                printf("%d. \e[1;32mobject %s type %s\e[0m\n", i, qstoc(o->objectName()), o->metaObject()->className());
                printf(" source: '%s' target '%s' \e[1;35mchild of %s \e[0;35m(%s)\e[0m\n", qstoc(o->property("source").toString()), qstoc(o->property("target").toString()),
                       o->parent() != nullptr ? qstoc(o->parent()->objectName()) : "(null)",
                       o->parent() != nullptr ? o->parent()->metaObject()->className() : "-");
                if(!QMetaObject::invokeMethod(o, "ctxSwitch", Q_ARG(CumbiaPool*, cu_pool), Q_ARG(CuControlsFactoryPool, m_ctrl_factory_pool)))
                    perr("error invoking method ctxSwitch on object %s type %s", qstoc(o->objectName()), o->metaObject()->className());
            }
        }
        return i;
    }

    int m_delete_cumbias(const std::vector<Cumbia *>& vc) {
        size_t i = 0;
        for(size_t i = 0; i < vc.size(); i++) {
            vc[i]->getServiceProvider()->unregisterService(CuServices::Log); // uninstall
            delete vc[i];
        }
        return i;
    }

    int engine_type(const CuContext *ctx) const {
        // get current engine in use
        Cumbia *c = nullptr;
        d->error = (ctx == nullptr);
        if(!d->error) {
            if(ctx->getReader())
                c = ctx->getReader()->getCumbia();
            else if(ctx->getWriter())
                c = ctx->getWriter()->getCumbia();
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

    bool same_engine(CumbiaPool *cu_pool, int engine) const {
        bool err = false;
        for(size_t i = 0; i < cu_pool->names().size() && !err; i++) {
            const std::string n = cu_pool->names().at(i);
            if(cu_pool->get(n))
                err = (cu_pool->get(n)->getType() == engine);
        }
        return err;
    }

    std::vector<Cumbia*> m_clear_cumbia(CumbiaPool* cu_pool, CuControlsFactoryPool &fp) {
        d->error = false;
        std::vector<Cumbia*> vc;
        // clear the pool
        for(size_t i = 0; i < cu_pool->names().size() && !d->error; i++) {
            const std::string n = cu_pool->names().at(i);
            d->error = !cu_pool->get(n);
            if(!d->error) {
                printf("unregistering cumbia impl '%s'\n", n.c_str());
                vc.push_back(cu_pool->get(n));
                cu_pool->unregisterCumbiaImpl(n);
                cu_pool->clearSrcPatterns(n);
            }
            else
                d->msg = QString("error getting cumbia instance for \"%1\"").arg(n.c_str());
        }
        if(!d->error)
            fp = CuControlsFactoryPool();
        return vc;
    }
};



#endif // QUAPPS_H
