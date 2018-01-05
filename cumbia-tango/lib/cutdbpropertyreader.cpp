#include "cutdbpropertyreader.h"
#include "cugettdbpropactivity.h"
#include <set>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <culog.h>
#include <cuactivity.h>
#include <list>
#include <cumbiatango.h>.h>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>

class CuTDbPropertyReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    std::list<CuData> in_props;
    std::string id;
    CumbiaTango *cumbia_t;
    CuGetTDbPropActivity *activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;
};

CuTDbPropertyReader::CuTDbPropertyReader(const std::string &id, CumbiaTango *cumbia)
{
    d = new CuTDbPropertyReaderPrivate;
    d->cumbia_t = cumbia;
    d->id = id;
}

CuTDbPropertyReader::~CuTDbPropertyReader()
{
    delete d;
}

void CuTDbPropertyReader::get(const std::list<CuData> &in_data)
{
    CuData thread_tok("id", d->id);
    d->activity = new CuGetTDbPropActivity(in_data);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, thread_tok, fi, bf);
}

void CuTDbPropertyReader::addListener(CuDataListener *l)
{
    d->listeners.insert(l);
}

void CuTDbPropertyReader::onProgress(int step, int total, const CuData &data)
{

}

void CuTDbPropertyReader::onResult(const CuData &data)
{
    for(std::set<CuDataListener *>::const_iterator it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(data);
}

CuData CuTDbPropertyReader::getToken() const
{

}

