#ifndef CUCONTROLSFACTORYPOOL_P_H
#define CUCONTROLSFACTORYPOOL_P_H

#include <map>
#include <string>
#include <vector>
#include <regex>
#include <atomic>

class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;

class CuControlsFactoryPool_P
{
public:
    CuControlsFactoryPool_P();
    CuControlsFactoryPool_P(const CuControlsFactoryPool_P& other);
    ~CuControlsFactoryPool_P();

    std::map<std::string, CuControlsReaderFactoryI *> rmap;
    std::map<std::string, CuControlsWriterFactoryI *> wmap;
    std::map<std::string, std::vector<std::string> > m_dom_patterns;
    std::map<std::string, std::vector<std::regex> > re_map;
    std::string default_domain;

    int ref() {
        return _r.fetch_add(1);
    }
    int unref() {
        return _r.fetch_sub(1);
    }
    int exchange(int desired) {
        return _r.exchange(desired);
    }
    int load() const {
        return _r.load();
    }

private:
    std::atomic<int> _r;
};

#endif // CUCONTROLSFACTORYPOOL_P_H
