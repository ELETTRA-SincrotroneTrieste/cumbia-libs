#ifndef CUENGINE_SWAP_H
#define CUENGINE_SWAP_H

#include <vector>

class QObject;
class QString;
class CuContext;
class Cumbia;
class CuDataListener;
class CumbiaPool;
class CuControlsFactoryPool;
class CuEngineSwap_P;

/*!
 * \brief Helper class to switch context on a controls reader (writer)
 *
 * \since 1.5.2
 */
class CuEngineSwap {
public:
    enum Engines { NoEngine = 0, Tango, Epics, Http, Random, Websocket, MaxEngines };

    CuEngineSwap();
    ~CuEngineSwap();

    CuContext *replace(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo);

    // all objects with either source or target property shall implement ctxSwap slot
    bool check_objs_have_ctx_swap(const QObject *root, QString *oclass, QString *onam) const;
    bool check_root_has_engine_accessor(const QObject *root) const;
    int ctx_swap(const QObject *root, CumbiaPool *cu_pool, const CuControlsFactoryPool &m_ctrl_factory_pool);
    bool app_engine_swap(const QObject *root, CumbiaPool *cu_pool, const CuControlsFactoryPool &fp);
    int engine_type(const CuContext *ctx) const;
    bool same_engine(CumbiaPool *cu_pool, int engine) const;
    bool log_move(const std::vector<Cumbia *> &vc, CumbiaPool *new_p) const;

    std::vector<Cumbia*> cumbia_clear(CumbiaPool* cu_pool, CuControlsFactoryPool &fp);
    int cumbias_delete(const std::vector<Cumbia *>& vc);

    bool ok() const;
    bool error() const;
    const QString& msg() const;

private:
    CuEngineSwap_P *d;


    // return a vector with all Cumbia instances that shall be disposed
    // unregister cumbia impls from the pool, and clear the src patterns
    // reset the factory pool to an empty one
    Cumbia * m_cumbia_get_from_r(const CuContext *ctx) const;
    Cumbia * m_cumbia_get_from_w(const CuContext *ctx) const;
};

#endif // CUENGINE_SWAP_H
