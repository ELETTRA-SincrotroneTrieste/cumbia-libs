#ifndef CUCTX_SWAP_H
#define CUCTX_SWAP_H

#include <vector>

class QObject;
class QString;
class CuContext;
class Cumbia;
class CuDataListener;
class CumbiaPool;
class CuControlsFactoryPool;
class CuEngineHotSwitchPrivate;

/*!
 * \brief Helper class to switch context on a controls reader (writer)
 *
 * \since 1.5.0
 */
class CuCtxSwap {
public:
    enum Engines { NoEngine = 0, Tango, Epics, Http, Random, Websocket, MaxEngines };

    CuCtxSwap();
    ~CuCtxSwap();

    CuContext *replace(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo);

    const char* engine_name(int engine) const;

    // all objects with either source or target property shall implement ctxSwap slot
    bool check_objs_have_ctx_swap(const QObject *root, QString *oclass, QString *onam) const;

    int swap(const QObject *root, CumbiaPool *cu_pool, const CuControlsFactoryPool &m_ctrl_factory_pool);

    int engine_type(const CuContext *ctx) const;

    bool same_engine(CumbiaPool *cu_pool, int engine) const;

    bool ok() const;
    bool error() const;
    const QString& msg() const;

private:
    CuEngineHotSwitchPrivate *d;

    int m_delete_cumbias(const std::vector<Cumbia *>& vc);

    // return a vector with all Cumbia instances that shall be disposed
    // unregister cumbia impls from the pool, and clear the src patterns
    // reset the factory pool to an empty one
    std::vector<Cumbia*> m_clear_cumbia(CumbiaPool* cu_pool, CuControlsFactoryPool &fp);
    Cumbia * m_cumbia_get_from_r(const CuContext *ctx) const;
    Cumbia * m_cumbia_get_from_w(const CuContext *ctx) const;
};

#endif // CUCTX_SWAP_H
