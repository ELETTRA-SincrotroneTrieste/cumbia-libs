#ifndef CUMBIATANGO_H
#define CUMBIATANGO_H

class CuTangoActionFactoryI;
class CuDataListener;

#include <cumbia.h>
#include <cutangoactioni.h>
#include<string>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;

class CumbiaTango : public Cumbia
{

public:
    enum Type { CumbiaTangoType = Cumbia::CumbiaUserType + 1 };

    CumbiaTango();

    CumbiaTango(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    void setThreadFactoryImpl( CuThreadFactoryImplI *tfi);

    void setThreadEventsBridgeFactory( CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaTango();

    void addAction(const std::string& source, CuDataListener *l, const CuTangoActionFactoryI &f);

    void unlinkListener(const std::string& source, CuTangoActionI::Type t, CuDataListener *l);

    CuTangoActionI *findAction(const std::string& source, CuTangoActionI::Type t) const;

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

private:

    void m_init();

    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIATANGO_H
