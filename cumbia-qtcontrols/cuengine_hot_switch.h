#ifndef CUENGINE_HOT_SWITCH_H
#define CUENGINE_HOT_SWITCH_H


class QObject;
class CuContext;
class CuDataListener;
class CumbiaPool;
class CuControlsFactoryPool;

/*!
 * \brief Helper class to switch context on a controls reader (writer)
 *
 * \since 1.5.0
 */
class CuEngineHotSwitch
{
public:
    CuContext *hot_switch(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo);
};

#endif // CUENGINE_HOT_SWITCH_H
