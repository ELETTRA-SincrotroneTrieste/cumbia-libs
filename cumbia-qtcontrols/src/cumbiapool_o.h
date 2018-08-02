#ifndef CUMBIAPOOL_O_H
#define CUMBIAPOOL_O_H

#include <QObject>

class CumbiaPool;
#include <cucontrolsfactorypool.h>

/*! \brief Wraps CumbiaPool and CuControlsFactoryPool in a QObject that can be used
 *         to set a context property on a qml application engine.
 *
 * \par Usage
 * Please refer to the cumbia-qtcontrols-qml plugin documentation for further details.
 * Here is an example of usage in the main.cpp of a qml application
 *
 * \code
 *
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>

#include <cucontrolsfactorypool.h>
#include <cumbiapool_o.h>
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutango-world.h>
#include <cumbiaepics.h>
#include <cuepics-world.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>  // for CuTWriterFactory
#include <cutcontrolsreader.h>  // for CuTReaderFactory
#include <cutcontrolswriter.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
 * int main(int argc, char *argv[])
{
    CumbiaPool *cu_pool = new CumbiaPool();
    CuControlsFactoryPool m_ctrl_factory_pool;

    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());

#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
#endif

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());

    CumbiaPoolFactory pf;
    pf.init(cu_pool, m_ctrl_factory_pool);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cumbia_poof", &pf);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));


    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
 * \endcode
 *
 */
class CumbiaPool_O : public QObject
{
    Q_OBJECT
public:
    explicit CumbiaPool_O(QObject *parent = nullptr);

    void init(CumbiaPool *cupoo, const CuControlsFactoryPool &fapool);

    virtual CumbiaPool *getPool() const;

    CuControlsFactoryPool getFactory() const;

public slots:

private:
    CumbiaPool *m_cu_pool;

    CuControlsFactoryPool m_fac;

};



#endif // CUMBIAPOOL_O_H
