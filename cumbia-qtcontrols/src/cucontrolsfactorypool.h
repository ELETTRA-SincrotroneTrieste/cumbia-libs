#ifndef CUCONTROLSFACTORYPOOL_H
#define CUCONTROLSFACTORYPOOL_H

class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;

#include <string>
#include <vector>
#include <map>
#include <regex>

/*! \brief this class, used in conjunction with CumbiaPool allows to connect to
 *         different control system frameworks within the same application.
 *
 * \ingroup core
 *
 * Reader and writer factories can be registered with registerImpl, providing a name
 * for the domain they refer to. The following code registers reader and writer
 * factories for *epics* and *tango*, making it possible to connect to sources
 * belonging to these two different systems within the same application:
 *
 * \code
 * ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
 * ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
 * m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
 * m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
 * \endcode
 *
 * The setSrcPatterns method can be used to insert rules so that CuControlsFactoryPool
 * can distribute sources among the registered engine implementations according to
 * their syntax pattern:
 *
 * \code
 * std::vector<std::string> epics_patterns, tango_patterns;
 * epics_patterns.push_back(".+:.+"); // e.g. control:ai1 (epics PV)
 * tango_patterns.push_back(".+/.+"); // e.g. test/device/1/double_scalar
 * tango_patterns.push_back(".+->.+"); // e.g. test/device/1->DevLong
 * // associate a domain ("tango" or "epics") to a specific pattern.
 * ctrl_factory_pool.setSrcPatterns("epics", epics_patterns);
 * ctrl_factory_pool.setSrcPatterns("tango", tango_patterns);
 * \endcode
 *
 */
class CuControlsFactoryPool
{
public:
    enum Type { Reader = 0, Writer };

    CuControlsFactoryPool();

    void registerImpl(const std::string &domain, const CuControlsReaderFactoryI &rf);

    void registerImpl(const std::string &domain, const CuControlsWriterFactoryI &wf);

    void setSrcPatterns(const std::string& domain, const std::vector<std::string> &regexps);

    void clearSrcPatterns(const std::string& domain);

    CuControlsReaderFactoryI *getReadFactory(const std::string& domain) const;

    CuControlsWriterFactoryI *getWriteFactory(const std::string& domain) const;

    CuControlsReaderFactoryI * getRFactoryBySrc(const std::string& src) const;

    CuControlsWriterFactoryI *getWFactoryBySrc(const std::string& src) const;

    std::string guessDomainBySrc(const std::string& src) const;

    bool isEmpty() const;

    std::vector<std::string> getSrcPatternDomains() const;

    std::vector<std::string>  getSrcPatterns(const std::string& domain) const;

private:
    std::map<std::string, CuControlsReaderFactoryI *> m_rmap;

    std::map<std::string, CuControlsWriterFactoryI *> m_wmap;

    std::map<std::string, std::vector<std::string> >m_dom_patterns;
    std::map<std::string, std::vector<std::regex> >m_remap;

    void m_print() const;

};

#endif // CUMBIAPOOL_H
