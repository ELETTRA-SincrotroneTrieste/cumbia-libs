#include "gettdbpropertyextension.h"
#include <cucontexti.h>
#include <cucontext.h>
#include <cumacros.h>
#include <cumbia.h>
#include <cumbiatango.h>
#include <cumbiapool.h>
#include <cutdbpropertyreader.h>
#include <QtDebug>

class GetTDbPropertyExtensionPrivate {
public:
    GetTDbPropertyExtensionPrivate(const CuContextI *cctx) : contexti(cctx) {}
    bool err;
    std::string msg;
    const CuContextI *contexti;
};

GetTDbPropertyExtension::GetTDbPropertyExtension(const CuContextI *ctx, QObject *parent)
    : QObject(parent)
{
    d = new GetTDbPropertyExtensionPrivate(ctx);
    d->err = false;
}

GetTDbPropertyExtension::~GetTDbPropertyExtension()
{
    printf("deleting GetTDbPropertyExtension %p\n", this);
    // do not delete context, it's just a reference to an object
    // managed by someone else
    delete d;
    printf("deleted\n");
}

QString GetTDbPropertyExtension::getName() const
{
    return "GetTDbPropertyExtension";
}

CuData GetTDbPropertyExtension::execute(const CuData &in, const CuContextI *ctx)
{
    qDebug() << __PRETTY_FUNCTION__ << in.toString().c_str() << ctx;
    d->contexti = ctx;
    CuData da;
    std::vector<CuData> in_list, out_list;
    in_list.push_back(in);
    out_list = execute(in_list, ctx);
    if(out_list.size() > 0)
        da = out_list[0];
    return da;
}

std::vector<CuData> GetTDbPropertyExtension::execute(const std::vector<CuData> &in_list, const CuContextI *ctxi)
{
    qDebug() << __PRETTY_FUNCTION__ << ctxi;

    d->contexti = ctxi;
    std::vector<CuData> out;
    out.push_back(CuData("msg", "property fetch in progress..."));
    CumbiaTango *cu_t = NULL;
    CumbiaPool *cu_poo = NULL;
    Cumbia *cu = d->contexti->getContext()->cumbia();
    if(!cu && (cu_poo = d->contexti->getContext()->cumbiaPool())) {
        cu = cu_poo->get("tango");
    }
    if(cu && cu->getType() == CumbiaTango::CumbiaTangoType)
        cu_t = static_cast<CumbiaTango *>(cu);
    if(cu_t && in_list.size() > 0) {
        CuTDbPropertyReader *tp_reader = new CuTDbPropertyReader("GetTDbPropertyExtension.execute", cu_t);
        tp_reader->addListener(this);
        tp_reader->get(in_list);
    }
    return out;
}

QObject *GetTDbPropertyExtension::get_qobject()
{
    return this;
}

const CuContextI *GetTDbPropertyExtension::getContextI() const
{
    return d->contexti;
}

void GetTDbPropertyExtension::onUpdate(const CuData &data)
{
    emit onDataReady(data);
}

std::string GetTDbPropertyExtension::message() const {
    return d->msg;
}

bool GetTDbPropertyExtension::error() const {
    return d->err;
}
