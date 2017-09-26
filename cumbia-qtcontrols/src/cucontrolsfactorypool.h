#ifndef CUCONTROLSFACTORYPOOL_H
#define CUCONTROLSFACTORYPOOL_H

class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;

#include <string>
#include <vector>
#include <map>

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

private:
    std::map<std::string, CuControlsReaderFactoryI *> m_rmap;

    std::map<std::string, CuControlsWriterFactoryI *> m_wmap;

    std::map<std::string, std::vector<std::string> >m_dom_patterns;

};

#endif // CUMBIAPOOL_H
