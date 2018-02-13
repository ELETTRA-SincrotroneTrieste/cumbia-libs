#ifndef CUMBIAPOOL_H
#define CUMBIAPOOL_H

#include <string>
#include <vector>
#include <map>

#include <cumbia.h>

/*! \brief used in combination with CuControlsFactoryPool, the class can be used to create applications
 *         that can connect to sources or targets belonging to different control system frameworks
 *
 * \ingroup core
 *
 * Declare CumbiaPool in your main class so that it's visible across the whole application.
 *
 * \code
  class GenericClient : public QWidget
  {
    Q_OBJECT
    public:
        // ...

    private:
        CumbiaPool *cu_pool; // to use in combination with CuControlsFactoryPool
        CuControlsFactoryPool m_ctrl_factory_pool;
  };
 * \endcode
 *
 * You need to instantiate the desired Cumbia engines and register them with registerCumbiaImpl.
 * Then you must call setSrcPatterns for each "domain" name (e.g. "tango" or "epics")
 * The following code shows an example. It integrates the considerations done in the
 * CuControlsFactoryPool documentation.
 *
 * \code
   // cumbia engine: CumbiaEpics
   CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
   // register cumbia engine with the "domain" name, a string used to identify it later
   cu_pool->registerCumbiaImpl("epics", cuep);
   // see CuControlsFactoryPool documentation
   m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
   m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
   CuEpicsWorld ew;
   // CuEpicsWorld.srcPatterns provides default patterns for the epics source patterns
   m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
   cu_pool->setSrcPatterns("epics", ew.srcPatterns());

   // tango engine: CumbiaTango
   CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
   cu_pool->registerCumbiaImpl("tango", cuta);
   m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
   m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
   // CuTangoWorld.srcPatterns provides default patterns for the tango source patterns
   CuTangoWorld tw;
   m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
   cu_pool->setSrcPatterns("tango", tw.srcPatterns());

   // ...
   ui->setupUi(this);

 * \endcode

 */
class CumbiaPool
{
public:
    CumbiaPool();

    void registerCumbiaImpl(const std::string &domain, Cumbia *cumbia);

    void setSrcPatterns(const std::string& domain, const std::vector<std::string> &regexps);

    void clearSrcPatterns(const std::string& domain);

    void unregisterCumbiaImpl(const std::string& domain);

    Cumbia *get(const std::string& domain) const;

    Cumbia *getBySrc(const std::string& src) const;

    Cumbia *guessBySrc(const std::string& src) const;

    bool isEmpty() const;

private:
    std::map<std::string, Cumbia *> m_map;

    std::map<std::string, std::vector<std::string> >m_dom_patterns;

};

#endif // CUMBIAPOOL_H
