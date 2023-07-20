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
    CuData options, tag;
    TSource tsrc;
};

CuTaDbActivity::CuTaDbActivity(const TSource &tsrc, const CuData &options, const CuData &tag)
    : CuActivity(CuData("activity", "cutadb").set("src", tsrc.getName()))
{
    d = new CuTaDbActivityPrivate;
    d->exiting = false;
    d->tsrc = tsrc;
    d->tag = tag;
    d->options = options;
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

void CuTaDbActivity::execute() {
    TSource::Type ty = d->tsrc.getType();
    CuData at("src", d->tsrc.getName()); /* activity token */
    at.merge(CuData(d->options));
    at["type"] = "property";
    at["op"] = d->tsrc.getTypeName(ty);
    at.merge(CuData(d->tag));
    CuTangoWorld w;
    w.db_get(d->tsrc, at);
    d->exiting = true;
    publishResult(at);
}

void CuTaDbActivity::onExit() { }
