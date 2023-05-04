#include "cuengineaccessor.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <qustringlist.h>

class CuEngineAccessorP {
public:
    CuEngineAccessorP(CumbiaPool **cum_p_p, CuControlsFactoryPool *fap_p)
        :  cu_p_p(cum_p_p), fp_p(fap_p) {}
    CumbiaPool **cu_p_p;
    CuControlsFactoryPool *fp_p;
};

CuEngineAccessor::CuEngineAccessor(QObject *parent, CumbiaPool **cu_p_p, CuControlsFactoryPool *fp_p)
    : QObject{parent}, d(new CuEngineAccessorP(cu_p_p, fp_p)) {
    pretty_pri("cumbia pool %p names %s", *(d->cu_p_p), qstoc(QuStringList((*(d->cu_p_p))->names()).join(",")));
}

CuEngineAccessor::~CuEngineAccessor() {
    delete d;
}

void CuEngineAccessor::engine_swap(CumbiaPool *cu_p, const CuControlsFactoryPool &fp) {
    QuStringList from(d->fp_p->getSrcPatternDomains());
    QuStringList to(fp.getSrcPatternDomains());
    printf("previous cumbia pool %p new %p\n", *(d->cu_p_p), cu_p);
    printf("old cumbia names %s new names %s\n", qstoc(from.join(",")),
            qstoc(QuStringList(fp.getSrcPatternDomains()).join(",")));
    if(*d->cu_p_p != cu_p) {
        delete *d->cu_p_p;
        *d->cu_p_p = cu_p;
    }
    if(from != to) {
        *d->fp_p = fp;
        emit engineSwapped();
        emit engineChanged(from, QuStringList(cu_p->names()));
    }
    else
        perr("CuEngineAccessor: same cumbia pool reference");
}
