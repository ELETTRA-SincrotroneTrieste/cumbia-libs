#include "cucontrolsfactorypool.h"
#include "cucontrolsfactories_i.h"
#include <cumacros.h>
#include <regex>
#include <QString>
#include <map>
#include <atomic>

CuControlsFactoryPool::CuControlsFactoryPool() : d(nullptr) {

}

CuControlsFactoryPool::CuControlsFactoryPool(const CuControlsFactoryPool &other) {
    d = other.d;
    if(d) {
        d->ref();
    }
}

bool CuControlsFactoryPool::operator ==(const CuControlsFactoryPool &other) const {
    // d->re_map is not used in comparison because there's no == between regex. yet we check patterns
    return d != nullptr && other.d != nullptr && other.d->default_domain == d->default_domain && other.d->m_dom_patterns == d->m_dom_patterns &&
           other.d->rmap == d->rmap && other.d->wmap == d->wmap;
}

CuControlsFactoryPool::CuControlsFactoryPool(CuControlsFactoryPool &&other) : d(other.d) {
    //    printf("CuControlsFactoryPool \e[0;32m move constructor from other %p other.d %p \e[0m\n", &other, other.d);
    other.d = nullptr;
}

CuControlsFactoryPool &CuControlsFactoryPool::operator=(const CuControlsFactoryPool &other) {
    if(this != &other) {
        if(other.d)
            other.d->ref();
        if(this->d && this->d->unref() == 1) {
            delete d;
        }
        d = other.d; // share
    }
    return *this;
}

CuControlsFactoryPool &CuControlsFactoryPool::operator=(CuControlsFactoryPool &&other) {
    if(this != &other) {
        if(d && d->unref() == 1) {
            delete d;
        }
        d = other.d;
        other.d = nullptr;
    }
    return *this;
}

void CuControlsFactoryPool::m_detach() {
    if(d && d->load() > 1) {
        d->unref();
        d = new CuControlsFactoryPool_P(*d);
    }
    else if(!d)
        d = new CuControlsFactoryPool_P;
}

CuControlsFactoryPool::~CuControlsFactoryPool() {
    if(d && d->unref() == 1) {
        delete d;
        d = nullptr;
    }
}

/*! \brief register a reader factory for a domain:
 *
 * @param domain a string identifying a domain, e.g. "tango" or "epics"
 * @param rf an instance of an object implementing CuControlsReaderFactoryI,
 *        such as CuTReaderFactory or CuEpReaderFactory
 *
 */
void CuControlsFactoryPool::registerImpl(const std::string &domain, const CuControlsReaderFactoryI &rf) {
    m_detach();
    d->rmap[domain] = rf.clone();
}

/*! \brief register a writer factory for a domain:
 *
 * @param domain a string identifying a domain, e.g. "tango" or "epics"
 * @param wf an instance of an object implementing CuControlsWriterFactoryI,
 *        such as CuTWriterFactory or CuEpWriterFactory
 *
 */
void CuControlsFactoryPool::registerImpl(const std::string &domain, const CuControlsWriterFactoryI &wf) {
    m_detach();
    d->wmap[domain] = wf.clone();
}

/*! \brief set or change the source patterns (regular expressions) for the domain
 *
 * @param domain  a string identifying a domain, e.g. "tango" or "epics"
 * @param regex_patt a vector of string with the set of regular expressions that are used to
 *        match and assign sources to engines.
 */
void CuControlsFactoryPool::setSrcPatterns(const std::string &domain,
                                           const std::vector<std::string> &regex_patt) {
    m_detach();
    for(const std::string& r : regex_patt) {
        d->re_map[domain].push_back(std::regex(r));
    }
    d->m_dom_patterns[domain] = regex_patt;
}

/*! \brief remove patterns for the given domain
 *
 * @param domain a string with the domain name whose patterns are to be removed
 */
void CuControlsFactoryPool::clearSrcPatterns(const std::string &domain) {
    m_detach();
    d->re_map.erase(domain);
    d->m_dom_patterns.erase(domain);
}

/*! \brief returns the reader factory that was registered with the given domain
 *
 * @param domain the name of the domain registered with registerImpl
 * @return the CuControlsReaderFactoryI implementation corresponding to the given domain, or
 *         NULL if the domain wasn't registered.
 */
CuControlsReaderFactoryI *CuControlsFactoryPool::getReadFactory(const std::string &domain) const {
    if(!d)
        return nullptr;
    std::map<std::string, CuControlsReaderFactoryI *>::const_iterator it = d->rmap.find(domain);
    if(it != d->rmap.end()) {
        return it->second;
    }
    perr("CuControlsFactoryPool.getReadFactory: no reader factory implementation registered with domain \"%s\"", domain.c_str());
    return nullptr;
}

/*! \brief returns the writer factory that was registered with the given domain
 *
 * @param domain the name of the domain registered with registerImpl
 * @return the CuControlsWriterFactoryI implementation corresponding to the given domain, or
 *         NULL if the domain wasn't registered.
 */
CuControlsWriterFactoryI *CuControlsFactoryPool::getWriteFactory(const std::string &domain) const
{
    if(!d)
        return nullptr;
    std::map<std::string, CuControlsWriterFactoryI *>::const_iterator it = d->wmap.find(domain);
    if(it != d->wmap.end())
        return it->second;
    perr("CuControlsFactoryPool.getWriteFactory: no writer factory implementation registered with domain \"%s\"", domain.c_str());
    return nullptr;
}

/** \brief if more than one factory is registered, uses the registered patterns to match the given
 *         source and returns a CuControlsReaderFactoryI, in case of pattern match, null otherwise
 *
 *
 * @param src the name of the source
 * @return a pointer to a registered CuControlsReaderFactoryI
 *
 * \par Domain matching.
 * If the source src contains the "://" substring, the domain is matched with the string preceding
 * the substring, and the corresponding factory implementation is returned.
 * Otherwise, if only one factory has been registered, it is returned without any further matching,
 * for the sake of performance. (since 1.5.1)
 * Otherwise, the domain is guessed matching src against the regular expression patterns registered
 * with setSrcPatterns.
 *
 * \note If no match is found, a null pointer is returned
 *
 */
CuControlsReaderFactoryI *CuControlsFactoryPool::getRFactoryBySrc(const std::string &src) const
{
    if(!d)
        return nullptr;
    CuControlsReaderFactoryI *rf = NULL;
    if(d->rmap.size() == 0)
        return rf;
    std::string domain;
    size_t pos = src.find("://");
    if(pos != std::string::npos) {
        domain = src.substr(0, pos);
    }
    else if(d->rmap.size() == 1) {
        //        printf("CuControlsFactoryPool %p::getRFactoryBySrc: (%s) only one \e[0;32mREADER FACTORY\e[0m registered (%s): no regex matching\n", this, src.c_str(), d->rmap.begin()->first.c_str());
        return d->rmap.begin()->second;
    }
    else {
        //        printf("CuControlsFactoryPool %p::getRFactoryBySrc: (%s) \e[1;35m%ld READER FACTORIES\e[0m registered: needs regex matching\n",
        //               this, src.c_str(), d->rmap.size());

        domain = guessDomainBySrc(src);

//        return defaultfa;
    }
    if(domain.length())
        return getReadFactory(domain);
    perr("CuControlsFactoryPool.getRFactoryBySrc: could not guess domain from \"%s\"" , src.c_str());
    m_print();
    return nullptr;
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
CuControlsWriterFactoryI *CuControlsFactoryPool::getWFactoryBySrc(const std::string &src) const {
    if(!d)
        return nullptr;
    CuControlsWriterFactoryI *wf = NULL;
    if(d->wmap.size() == 0)
        return wf;

    std::string domain;
    size_t pos = src.find("://");
    if(pos != std::string::npos)
        domain = src.substr(0, pos);
    else if(d->wmap.size() == 1)
        return d->wmap.begin()->second;
    else
        domain = guessDomainBySrc(src);

    if(domain.length())
        return getWriteFactory(domain);

    perr("CuControlsFactoryPool.getWFactoryBySrc: could not guess domain from \"%s\":\n", src.c_str());
    return nullptr; // return the default cumbia impl (the first registered)
}

/*! \brief given the source string, , tries to match it with the
 *         registered patterns in order to return the associated domain name
 *
 * @param src the source name
 * @return a string with the guessed domain or an empty string if no regexp match has been found.
 */
std::string CuControlsFactoryPool::guessDomainBySrc(const std::string &src) const {
    if(!d)
        return nullptr;
    std::map<std::string, std::vector<std::regex> >::const_iterator it;
    for(it = d->re_map.begin(); it != d->re_map.end(); ++it) {
        for(const std::regex& re : it->second) {
            if(std::regex_match(src, re))
                return it->first; // get by domain name
        }
    }
    return "";
}

/** \brief returns true if both the reader factory map and the writer factory map are empty.
 *
 * @return  true if both the reader factory map and the writer factory map are empty, false otherwise.
 */
bool CuControlsFactoryPool::isEmpty() const {
    return !d || (d->rmap.size() == 0 && d->wmap.size() == 0);
}

std::vector<std::string> CuControlsFactoryPool::getSrcPatternDomains() const {
    std::vector<std::string> domains;
    if(!d)
        return domains;
    std::map<std::string, std::vector<std::string> >::const_iterator it;
    for(it = d->m_dom_patterns.begin(); it != d->m_dom_patterns.end(); ++it)
        domains.push_back(it->first);
    return domains;
}

std::vector<std::string> CuControlsFactoryPool::getSrcPatterns(const std::string &domain) const {
    if(!d)
        return std::vector<std::string>();
    std::map<std::string, std::vector<std::string> >::const_iterator it = d->m_dom_patterns.find(domain);
    return it != d->m_dom_patterns.end() ? it->second : std::vector<std::string>();
}

void CuControlsFactoryPool::setDefaultDomain(const std::string &dom) {
    m_detach();
    d->default_domain = dom;
}

std::string CuControlsFactoryPool::defaultDomain() const {
    return (d != nullptr) ? d->default_domain : std::string();
}

void CuControlsFactoryPool::m_print() const {
    printf("CuControlsFactoryPool: domains and patterns:\n");
    if(!d) {
        printf("none\n");
    }
    else {
        std::map<std::string, std::vector<std::string> >::const_iterator it;
        for(it = d->m_dom_patterns.begin(); it != d->m_dom_patterns.end(); ++it) {
            printf("'%s' -> patterns {", it->first.c_str());
            for(size_t i = 0; i < it->second.size(); i++)
                printf("'%s',", it->second[i].c_str());
            printf("\n");
        }
    }
}

