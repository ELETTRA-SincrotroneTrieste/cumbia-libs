#ifndef CUMBIAPOOL_H
#define CUMBIAPOOL_H

#include <string>
#include <vector>
#include <map>

#include <cumbia.h>

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
