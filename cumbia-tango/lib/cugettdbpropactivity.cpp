#include "cugettdbpropactivity.h"
#include <cutango-world.h>
#include <cumacros.h> // for print

class CuGetTDbPropActivityPrivate
{
public:
    std::list<CuData> in_data;
};

CuGetTDbPropActivity::CuGetTDbPropActivity(const std::list<CuData> &in_data)
{
    d = new CuGetTDbPropActivityPrivate;
    d->in_data = in_data;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

CuGetTDbPropActivity::~CuGetTDbPropActivity()
{
    pdelete("~CuGetTDbPropActivity %p", this);
    delete d;
}

void CuGetTDbPropActivity::event(CuActivityEvent *e)
{

}

bool CuGetTDbPropActivity::matches(const CuData &token) const
{
    return getToken() == token;
}

void CuGetTDbPropActivity::init()
{
}

void CuGetTDbPropActivity::execute()
{
    CuTangoWorld tw;
    CuData res;
    tw.get_properties(d->in_data, res);
    publishResult(res);
}

void CuGetTDbPropActivity::onExit()
{

}
