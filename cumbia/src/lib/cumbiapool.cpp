#include "cumbiapool.h"
#include <cumacros.h>
#include <regex>

/*! \brief the class constructor
 *
 * Creates an empty CumbiaPool with no registered implementations
 *
 * \note
 * This class is used in combination with cumbia-qtcontrols CuControlsFactoryPool to set up
 * a multi engine environment for a cumbia/Qt application.
 * cumbia-qtcontrols CuControlsFactoryPool documentation is a recommended reading.
 */
CumbiaPool::CumbiaPool()
{

}

/*! \brief register a cumbia implementation with a given domain
 *
 * @param domain a string identifying a domain name, e.g. "tango" or "epics"
 * @param cumbia, an instance of a Cumbia implementation, such as CumbiaEpics or CumbiaTango.
 */
void CumbiaPool::registerCumbiaImpl(const std::string &domain, Cumbia *cumbia)
{
    m_map[domain] = cumbia;
}

/*! \brief associate to a domain name a list of regular expressions describing the source syntax
 *         for that domain.
 *
 * \code
 * std::vector<std::string> epics_patterns, tango_patterns;
 * tango_patterns.push_back(".+/.+"); // e.g. test/device/1/double_scalar
 * tango_patterns.push_back(".+->.+"); // e.g. test/device/1->DevLong
 * cu_pool->setSrcPatterns("tango", tango_patterns);
 * \endcode
 *
 */
void CumbiaPool::setSrcPatterns(const std::string &domain, const std::vector<std::string> &regexps)
{
    m_dom_patterns[domain] = regexps;
}

/*! \brief remove the source patterns for the given domain
 *
 * @param domain the domain name whose patterns will be removed
 */
void CumbiaPool::clearSrcPatterns(const std::string &domain)
{
    m_dom_patterns.erase(domain);
}

/*! \brief remove the domain from the list of the registered ones.
 *
 * @param domain the domain with this name will be removed from the CumbiaPool domain list
 */
void CumbiaPool::unregisterCumbiaImpl(const std::string &domain)
{
    if(m_map.count(domain) > 0)
        m_map.erase(domain);
}

/** \brief get the Cumbia implementation that was registered with the given domain name
 *
 * @param domain the name of the domain associated with a Cumbia implementation through a previous
 *        call to registerCumbiaImpl
 *
 * @return the Cumbia implementation for the given domain, if it had been registered
 * @return the first registered Cumbia implementation if no implementation has been found with that domain
 *         name
 * @return NULL if no Cumbia implementation have been registered in the CumbiaPool
 */
Cumbia *CumbiaPool::get(const std::string &domain) const
{
    std::map<std::string, Cumbia *>::const_iterator it = m_map.find(domain);
    if(it != m_map.end())
        return it->second;
    return nullptr;
}

/** \brief Calls get with the domain extracted from the full source name "src"
 *
 * @param src the name of the data source. Must begin with the <em>domain name</em>
 *        followed by <em>://</em>, for example <strong><cite>epics://</cite>foo:bar</strong>
 *
 */
Cumbia *CumbiaPool::getBySrc(const std::string &src) const
{
    Cumbia *cu = NULL;
    if(m_map.size() == 0)
        return cu;

    size_t pos = src.find("://");
    if(pos != std::string::npos)
        cu = get(src.substr(0, pos));

    if(!cu)
        cu = guessBySrc(src);

    if(!cu) {
        perr("CumbiaPool.getBySrc: could not guess domain from \"%s\"", src.c_str());
        m_print_registered_domain_info();
    }

    return cu;
}

/*! \brief given the source string, tries to match it with the
 *         registered patterns in order to return the associated Cumbia implementation.
 *
 * @param src the source name
 * @return a Cumbia implementation, if one was found matching src, or NULL otherwise.
 */
Cumbia *CumbiaPool::guessBySrc(const std::string &src) const
{
    if(m_map.size() == 0)
        return NULL;

    std::map<std::string, std::vector<std::string> >::const_iterator it;
    for(it = m_dom_patterns.begin(); it != m_dom_patterns.end(); ++it)
    {
        const std::vector<std::string> &patterns = it->second;
        for(size_t i = 0; i < patterns.size(); i++)
        {
            if(std::regex_match(src, std::regex(patterns.at(i))))
                return get(it->first); // get by domain name
        }
    }
    return NULL;
}

/*! \brief returns true if the CumbiaPool has no registered domains/Cumbia implementations
 *
 * @return  true if the CumbiaPool has no registered domains/Cumbia implementations, false otherwise
 */
bool CumbiaPool::isEmpty() const
{
    return m_map.size() == 0;
}

void CumbiaPool::m_print_registered_domain_info() const
{
    int i = 0;
    std::map<std::string, std::vector<std::string> >::const_iterator it;
    printf("   CumbiaPool: registered domains and patterns:\n");
    for(it = m_dom_patterns.begin(); it != m_dom_patterns.end(); ++it)
    {
        printf("      - domain\t%d: %s -> ", ++i, it->first.c_str());
        const std::vector<std::string> &patterns = it->second;
        for(size_t i = 0; i < patterns.size(); i++)
            i < patterns.size() - 1 ? printf("%s, ", patterns[i].c_str()) : printf("%s\n", patterns[i].c_str());
    }
}


