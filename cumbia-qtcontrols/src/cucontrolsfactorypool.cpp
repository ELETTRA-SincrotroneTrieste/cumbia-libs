#include "cucontrolsfactorypool.h"
#include "cucontrolsfactories_i.h"
#include <cumacros.h>
#include <regex>

/*! the class constructor
 *
 * \par example
 * Header file
 * \code
  class GenericClient : public QWidget
  {
    Q_OBJECT
    public:
        // ...

    private:
        CumbiaPool *cu_pool; // to use in combination with CuControlsFactoryPool
        CuControlsFactoryPool m_ctrl_factory_pool;
 * \endcode
 */
CuControlsFactoryPool::CuControlsFactoryPool()
{

}

/*! \brief register a reader factory for a domain:
 *
 * @param domain a string identifying a domain, e.g. "tango" or "epics"
 * @param rf an instance of an object implementing CuControlsReaderFactoryI,
 *        such as CuTReaderFactory or CuEpReaderFactory
 *
 */
void CuControlsFactoryPool::registerImpl(const std::string &domain, const CuControlsReaderFactoryI &rf)
{
    m_rmap[domain] = rf.clone();
}

/*! \brief register a writer factory for a domain:
 *
 * @param domain a string identifying a domain, e.g. "tango" or "epics"
 * @param wf an instance of an object implementing CuControlsWriterFactoryI,
 *        such as CuTWriterFactory or CuEpWriterFactory
 *
 */
void CuControlsFactoryPool::registerImpl(const std::string &domain, const CuControlsWriterFactoryI &wf)
{
    m_wmap[domain] = wf.clone();
}

/*! \brief set or change the source patterns (regular expressions) for the domain
 *
 * @param domain  a string identifying a domain, e.g. "tango" or "epics"
 * @param regex_patt a vector of string with the set of regular expressions that are used to
 *        match and assign sources to engines.
 */
void CuControlsFactoryPool::setSrcPatterns(const std::string &domain,
                                              const std::vector<std::string> &regex_patt)
{
    m_dom_patterns[domain] = regex_patt;
}

/*! \brief remove patterns for the given domain
 *
 * @param domain a string with the domain name whose patterns are to be removed
 */
void CuControlsFactoryPool::clearSrcPatterns(const std::string &domain)
{
    m_dom_patterns.erase(domain);
}

/*! \brief returns the reader factory that was registered with the given domain
 *
 * @param domain the name of the domain registered with registerImpl
 * @return the CuControlsReaderFactoryI implementation corresponding to the given domain, or
 *         NULL if the domain wasn't registered.
 */
CuControlsReaderFactoryI *CuControlsFactoryPool::getReadFactory(const std::string &domain) const
{
    std::map<std::string, CuControlsReaderFactoryI *>::const_iterator it = m_rmap.find(domain);
    if(it != m_rmap.end())
        return it->second;
    if(m_rmap.size() > 0)
        return m_rmap.begin()->second;
    perr("CuControlsFactoryPool.getReadFactory: no CuControlsReaderFactoryI implementation registered with domain \"%s\"", domain.c_str());
    return NULL;
}

/*! \brief returns the writer factory that was registered with the given domain
 *
 * @param domain the name of the domain registered with registerImpl
 * @return the CuControlsWriterFactoryI implementation corresponding to the given domain, or
 *         NULL if the domain wasn't registered.
 */
CuControlsWriterFactoryI *CuControlsFactoryPool::getWriteFactory(const std::string &domain) const
{
    std::map<std::string, CuControlsWriterFactoryI *>::const_iterator it = m_wmap.find(domain);
    if(it != m_wmap.end())
        return it->second;
    if(m_wmap.size() > 0)
        return m_wmap.begin()->second;
    perr("CuControlsFactoryPool.getWriteFactory: no CuControlsWriterFactoryI implementation registered with domain \"%s\"", domain.c_str());
    return NULL;
}

/** \brief uses the registered patterns to match the given source and return a CuControlsReaderFactoryI
 *         if a correspondence is found.
 *
 * @param src the name of the source
 * @return a CuControlsReaderFactoryI implementation matching the source.
 *
 * \par Domain matching.
 * If the source src contains the "://" substring, the domain is matched with the string preceding
 * the substring, and the corresponding factory implementation is returned.
 * Otherwise, the domain is guessed matching src against the regular expression patterns registered
 * with setSrcPatterns.
 *
 * \note If no match is found, a random CuControlsReaderFactoryI implementation may be returned
 * (the first stored in the std::map).
 *
 */
CuControlsReaderFactoryI *CuControlsFactoryPool::getRFactoryBySrc(const std::string &src) const
{
    CuControlsReaderFactoryI *rf = NULL;
    if(m_rmap.size() == 0)
        return rf;

    std::string domain;
    size_t pos = src.find("://");
    if(pos != std::string::npos)
        domain = src.substr(0, pos);
    else
        domain = guessDomainBySrc(src);

    if(domain.length())
        return getReadFactory(domain);

    perr("CuControlsFactoryPool.getRFactoryBySrc: could not guess domain from \"%s\":\n"
         "this may have unwanted side effects: returning factory for \"%s\"\n", src.c_str(),
         m_rmap.begin()->first.c_str());
    return m_rmap.begin()->second; // return the default cumbia impl (the first registered)
}

/** \brief uses the registered patterns to match the given source and return a CuControlsWriterFactoryI
 *         if a correspondence is found.
 *
 * @param src the name of the source
 * @return a CuControlsWriterFactoryI implementation matching the source.
 *
 * \par Domain matching.
 * If the source src contains the "://" substring, the domain is matched with the string preceding
 * the substring, and the corresponding factory implementation is returned.
 * Otherwise, the domain is guessed matching src against the regular expression patterns registered
 * with setSrcPatterns.
 *
 * \note If no match is found, a random CuControlsReaderFactoryI implementation may be returned
 * (the first stored in the std::map).
 *
 */
CuControlsWriterFactoryI *CuControlsFactoryPool::getWFactoryBySrc(const std::string &src) const
{
    CuControlsWriterFactoryI *wf = NULL;
    if(m_wmap.size() == 0)
        return wf;

    std::string domain;
    size_t pos = src.find("://");
    if(pos != std::string::npos)
        domain = src.substr(0, pos);
    else
        domain = guessDomainBySrc(src);

    if(domain.length())
        return getWriteFactory(domain);

    perr("CuControlsFactoryPool.getWFactoryBySrc: could not guess domain from \"%s\":\n"
         "this may have unwanted side effects: returning factory for \"%s\"\n", src.c_str(),
         m_wmap.begin()->first.c_str());
    return m_wmap.begin()->second; // return the default cumbia impl (the first registered)
}

/*! \brief given the source string, , tries to match it with the
 *         registered patterns in order to return the associated domain name
 *
 * @param src the source name
 * @return a string with the guessed domain or an empty string if no regexp match has been found.
 */
std::string CuControlsFactoryPool::guessDomainBySrc(const std::string &src) const
{
    std::map<std::string, std::vector<std::string> >::const_iterator it;
    for(it = m_dom_patterns.begin(); it != m_dom_patterns.end(); ++it)
    {
        const std::vector<std::string> &patterns = it->second;
        for(size_t i = 0; i < patterns.size(); i++)
        {
            if(std::regex_match(src, std::regex(patterns.at(i))))
                return it->first; // get by domain name
        }
    }
    return "";
}

/** \brief returns true if both the reader factory map and the writer factory map are empty.
 *
 * @return  true if both the reader factory map and the writer factory map are empty, false otherwise.
 */
bool CuControlsFactoryPool::isEmpty() const
{
    return m_rmap.size() == 0 || m_wmap.size() == 0;
}

