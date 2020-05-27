#include "cutadbactivity.h"
#include <tango.h>
#include <cumacros.h>
#include <algorithm>
#include "cutango-world.h"
#include "tsource.h"

class CuTaDbActivityPrivate
{
public:
    bool exiting;
    CuData options;
    TSource tsrc;
};

CuTaDbActivity::CuTaDbActivity(const CuData &tok, const TSource &tsrc) : CuActivity(tok)
{
    d = new CuTaDbActivityPrivate;
    d->exiting = false;
    d->tsrc = tsrc;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

CuTaDbActivity::~CuTaDbActivity()
{
    pdelete("CuTAttConfigActivity %p", this);
    delete d;
}

void CuTaDbActivity::setOptions(const CuData &o) {
    d->options = o;
}

int CuTaDbActivity::getType() const
{
    return CuTaDbActivityTypeRead;
}

void CuTaDbActivity::event(CuActivityEvent *e)
{
    (void )e;
}

bool CuTaDbActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

int CuTaDbActivity::repeat() const
{
    return -1;
}

void CuTaDbActivity::init() {

}

void CuTaDbActivity::execute()
{
    CuData at = getToken(); /* activity token */
    TSource::Type ty = d->tsrc.getType();
    at["type"] = "property";
    at["op"] = d->tsrc.getTypeName(ty);
    at.merge(d->options);
    CuTangoWorld w;
    w.db_get(d->tsrc, at);
    publishResult(at);
}

void CuTaDbActivity::onExit()
{
    CuData at = getToken(); /* activity token */
    if(!d->exiting) {
        d->exiting = true;
    }
    at["exit"] = true;
    publishResult(at);
}
