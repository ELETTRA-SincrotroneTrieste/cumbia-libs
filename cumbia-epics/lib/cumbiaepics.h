#ifndef CUMBIAEPICS_H
#define CUMBIAEPICS_H

class CuEpicsActionFactoryI;
class CuDataListener;

#include <cumbia.h>
#include "cuepactioni.h"
#include<string>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;

class CumbiaEpics : public Cumbia
{

public:
    enum Type { CumbiaEpicsType = Cumbia::CumbiaUserType + 1 };

    CumbiaEpics();

    CumbiaEpics(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    void setThreadFactoryImpl( CuThreadFactoryImplI *tfi);

    void setThreadEventsBridgeFactory( CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaEpics();

    void addAction(const std::string& source, CuDataListener *l, const CuEpicsActionFactoryI &f);

    void unlinkListener(const std::string& source, CuEpicsActionI::Type t, CuDataListener *l);

    CuEpicsActionI *findAction(const std::string& source, CuEpicsActionI::Type t) const;

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

private:

    void m_init();

    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIAEPICS_H
