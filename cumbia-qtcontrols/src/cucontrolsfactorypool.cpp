#include "cucontrolsfactorypool.h"
#include "cucontrolsfactories_i.h"
#include <cumacros.h>
#include <regex>

CuControlsFactoryPool::CuControlsFactoryPool()
{

}

void CuControlsFactoryPool::registerImpl(const std::string &domain, const CuControlsReaderFactoryI &rf)
{
    m_rmap[domain] = rf.clone();
}

void CuControlsFactoryPool::registerImpl(const std::string &domain, const CuControlsWriterFactoryI &wf)
{
    m_wmap[domain] = wf.clone();
}

void CuControlsFactoryPool::setSrcPatterns(const std::string &domain,
                                              const std::vector<std::string> &regex_patt)
{
    m_dom_patterns[domain] = regex_patt;
}

void CuControlsFactoryPool::clearSrcPatterns(const std::string &domain)
{
    m_dom_patterns.erase(domain);
}

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

std::string CuControlsFactoryPool::guessDomainBySrc(const std::string &src) const
{
    std::map<std::string, std::vector<std::string> >::const_iterator it;
    for(it = m_dom_patterns.begin(); it != m_dom_patterns.end(); ++it)
    {
        const std::vector<std::string> &patterns = it->second;
        for(size_t i = 0; i < patterns.size(); i++)
        {
            printf("CuControlsFactoryPool: matching %s with pattern %s success %d\n", src.c_str(), patterns.at(i).c_str(),
                   std::regex_match(src, std::regex(patterns.at(i))));
            if(std::regex_match(src, std::regex(patterns.at(i))))
                return it->first; // get by domain name
        }
    }
    return "";
}

bool CuControlsFactoryPool::isEmpty() const
{
    return m_rmap.size() == 0 || m_wmap.size() == 0;
}

