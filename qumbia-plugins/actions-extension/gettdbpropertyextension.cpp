#include "gettdbpropertyextension.h"
#include <cucontext.h>
#include <cumacros.h>

class GetTDbPropertyExtensionPrivate {
public:
    CuContext *context;
};

GetTDbPropertyExtension::GetTDbPropertyExtension(CuContext *ctx, QObject *parent)
 : QObject(parent)
{
    d = new GetTDbPropertyExtensionPrivate;
    d->context = ctx;
}

GetTDbPropertyExtension::~GetTDbPropertyExtension()
{
    // do not delete context, it's just a reference to an object
    // managed by someone else
    delete d;
}

QString GetTDbPropertyExtension::getName() const
{
    return "GetTDbPropertyExtension";
}

CuData GetTDbPropertyExtension::execute(const CuData &in)
{
    CuData out(in);
    out["out"] = "is out!";
    printf("GetTDbPropertyExtension::execute: \e[0;32min: %s --> out: \e[1;32m%s\e[0m\n",
           in.toString().c_str(), out.toString().c_str());

    emit onDataReady(out);
    return out;
}

QObject *GetTDbPropertyExtension::get_qobject()
{
    return this;
}


