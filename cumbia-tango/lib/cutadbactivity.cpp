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

    if(tsrc.getName().find("beamdump_s*") != std::string::npos)
        printf("CuTaDbActivity::CuTaDbActivity %s\n", tsrc.getName().c_str());
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
    printf("CuTaDbActivity.\e[0;33mmatches: %s vs mytok %s\e[0m\n", datos(getToken()), datos(token));
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
    if(d->tsrc.getName().find("beamdump_s*") != std::string::npos)
        printf("\e[0;36mCuTaDbActivity.execute: ENTERING %s\e[0m\n", d->tsrc.getName().c_str());
    TSource::Type ty = d->tsrc.getType();
    CuData at("src", d->tsrc.getName()); /* activity token */
    at.merge(CuData(d->options));
    at["type"] = "property";
    at["op"] = d->tsrc.getTypeName(ty);
    at.merge(CuData(d->tag));
    CuTangoWorld w;
    w.db_get(d->tsrc, at);
    d->exiting = true;
    if(d->tsrc.getName().find("beamdump_s*") != std::string::npos)
        printf("\e[0;36mCuTaDbActivity.execute: publishing result for %s\e[0m\n", d->tsrc.getName().c_str());
    publishResult(at);
}

void CuTaDbActivity::onExit() { }
