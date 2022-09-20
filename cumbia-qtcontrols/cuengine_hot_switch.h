#ifndef CUENGINE_HOT_SWITCH_H
#define CUENGINE_HOT_SWITCH_H


class QObject;
class QString;
class CuContext;
class CuDataListener;
class CumbiaPool;
class CuControlsFactoryPool;
class CuEngineHotSwitchPrivate;

/*!
 * \brief Helper class to switch context on a controls reader (writer)
 *
 * \since 1.5.0
 */
class CuEngineHotSwitch {
public:
    enum Engines { NoEngine = 0, Tango, Epics, Http, Random, Websocket, MaxEngines };

    CuEngineHotSwitch();
    ~CuEngineHotSwitch();

    CuContext *hot_switch(CuDataListener *l, CuContext *ctx, CumbiaPool *p, const CuControlsFactoryPool &fpoo);
    void switch_engine(int engine, CumbiaPool *p, const CuControlsFactoryPool &fpoo);
    int detect_engine(CuContext *ctx) const;

    const char* engine_name(int engine) const;

    bool error() const;
    const QString& msg() const;

private:
    CuEngineHotSwitchPrivate *d;

};

#endif // CUENGINE_HOT_SWITCH_H
