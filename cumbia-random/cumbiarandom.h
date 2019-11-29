#ifndef CUMBIARANDOM_H
#define CUMBIARANDOM_H

#include <cumbia.h>
#include <string>
#include <curndactionreader.h>
#include <curndactionfactoryi.h>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;

class CumbiaRandom : public Cumbia
{

public:
    enum Type { CumbiaRNDType = Cumbia::CumbiaUserType + 12 };

    CumbiaRandom(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaRandom();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

    void addAction(const std::string &source, CuDataListener *l, const CuRNDActionFactoryI &f);
    void unlinkListener(const string &source, CuRNDActionI::Type t, CuDataListener *l);
    CuRNDActionI *findAction(const std::string &source, CuRNDActionI::Type t) const;
private:

    void m_init();
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIARANDOM_H
