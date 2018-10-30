#include "infodialogextension.h"
#include "cuinfodialog.h"

#include <cucontext.h>

class InfoDialogExtensionPrivate {
public:
    InfoDialogExtensionPrivate(const CuContext *cctx) : ctx(cctx) {}
    std::string msg;
    bool err;
    const CuContext *ctx;
};

InfoDialogExtension::InfoDialogExtension(const CuContext *ctx, QObject *parent) : QObject(parent)
{
    d = new InfoDialogExtensionPrivate(ctx);
    d->err = false;
}

InfoDialogExtension::~InfoDialogExtension()
{
    delete d;
}

QString InfoDialogExtension::getName() const
{
    return "InfoDialogExtension";
}

CuData InfoDialogExtension::execute(const CuData &in)
{
#ifdef CUMBIAQTCONTROLS_HAS_QWT
    // WA_DeleteOnClose attribute is set
    CuInfoDialog* dlg = new CuInfoDialog(0, d->ctx);
    dlg->exec(in);
#else
#endif
    return CuData();
}

std::vector<CuData> InfoDialogExtension::execute(const std::vector<CuData>& )
{
    return std::vector<CuData>();
}

QObject *InfoDialogExtension::get_qobject()
{
    return this;
}

const CuContext *InfoDialogExtension::getContext() const
{
    return d->ctx;
}


std::string InfoDialogExtension::message() const{
    return d->msg;
}

bool InfoDialogExtension::error() const {
    return d->err;
}
