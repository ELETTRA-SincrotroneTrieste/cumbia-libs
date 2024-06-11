#ifndef QUAPPS_H
#define QUAPPS_H

#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <culog.h>
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>
#include <cuengineaccessor.h>

#include <QStringList>
#include <QMetaProperty>
#include <QMetaMethod>

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
        m_register(cu_pool, ctrl_f_pool);
    }

    CuModuleLoader(CuEngineAccessor *a, CuLogImplI* log_impl = nullptr) {
        if(a && a->cu_pool() && a->f_pool()) {
            d = new CuModuleLoaderPrivate;
            log_impl ? d->log = new CuLog(log_impl) : d->log = nullptr;
            m_register(a->cu_pool(), a->f_pool());
        }
        else {
            perr("CuModuleLoader: either accessor or cumbia pool are invalid");
        }
    }

    void m_register(CumbiaPool *cu_pool, CuControlsFactoryPool *ctrl_f_pool) {
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

    /*!
     * \brief prepare a new CumbiaPool and a CuControlsFactoryPool with the requested engine
     *
     * cumbia-tango and cumbia-http engines only are supported.
     * \param engine the engine identifier, either CumbiaHttp::CumbiaHTTPType && engine != CumbiaTango::CumbiaTangoType
     * \param m_ctrl_factory_pool a reference to a CuControlsFactoryPool that will be initialized
     *
     * \return a *new* CumbiaPool with the required new engine or nullptr in case of error
     *
     * Check for nullptr return type and get the error message with the msg method.
     */
    CumbiaPool* prepare_engine(int engine, CuControlsFactoryPool *m_ctrl_factory_pool) {
        CumbiaPool *cu_pool = nullptr;
        QString engine_name;
        d->error = true;
#ifdef CUMBIA_HTTP_VERSION
        if(engine == CumbiaHttp::CumbiaHTTPType) {
            d->error = false;
            engine_name = "http";
        }
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        if(engine == CumbiaTango::CumbiaTangoType) {
            d->error = false;
            engine_name = "tango native";
        }
#endif
        // supported engines: Tango and Http
        if(d->error)
            d->msg = "engine hot switch supports the Tango and Http engines only";
        else { // switch to the new engine
            // save the log instance
            // when engine switching, we suppose there is only one Cumbia in the pool
            // because interchangeable engines are mutually exclusive (see cclear.size() == 1 below)
            //
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
            if(engine == CumbiaTango::CumbiaTangoType) {
                cu_pool = new CumbiaPool();
                CuTangoRegisterEngine tare;
                tare.registerWithDefaults(cu_pool,  *m_ctrl_factory_pool);
            }
#endif
#ifdef CUMBIA_HTTP_VERSION
            if(engine == CumbiaHttp::CumbiaHTTPType) {
                CuHttpRegisterEngine httpre;
                d->error = !httpre.load(qApp->arguments(), true); // true: try loading also without -u in args
                if(!d->error) {
                    cu_pool = new CumbiaPool();
                    httpre.registerWithDefaults(cu_pool, *m_ctrl_factory_pool);
                }
                else
                    d->msg = "error registering http module: valid URL '" + httpre.url() + "' ?";
            }
#endif
            for(size_t i = 0; cu_pool != nullptr && i < cu_pool->names().size(); i++) {
                const std::string n = cu_pool->names().at(i);
                printf("+ [quapps]: cumbia registered \e[1;32m%s\e[0m type \e[0;36m%s\e[0m\n", n.c_str(), qstoc(engine_name));
            }
        }  // ! error unsupported target engine
        return cu_pool;
    }

};

#endif // QUAPPS_H
