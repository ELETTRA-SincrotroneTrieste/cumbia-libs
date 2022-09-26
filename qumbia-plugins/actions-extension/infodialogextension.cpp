#include "infodialogextension.h"
#include "cuinfodialog.h"

#include <cucontext.h>

class InfoDialogExtensionPrivate {
public:
    InfoDialogExtensionPrivate(const CuContextI *cctx) : ctx(cctx) {}
    std::string msg;
    bool err;
    const CuContextI *ctx;
};

InfoDialogExtension::InfoDialogExtension(const CuContextI *ctxi, QObject *parent) : QObject(parent)
{
    d = new InfoDialogExtensionPrivate(ctxi);
    d->err = false;
}

InfoDialogExtension::~InfoDialogExtension()
{
    printf("deleting InfoDialogExtension\n");
    delete d;
    printf("deleted\n");
}

QString InfoDialogExtension::getName() const {
    return "InfoDialogExtension";
}

CuData InfoDialogExtension::execute(const CuData &in, const CuContextI *ctxI)
{
#ifdef CUMBIAQTCONTROLS_HAS_QWT
    // WA_DeleteOnClose attribute is set
    CuInfoDialog* dlg = new CuInfoDialog(nullptr);
    dlg->exec(in, ctxI);
#else
#endif
    return CuData();
}

std::vector<CuData> InfoDialogExtension::execute(const std::vector<CuData>&, const CuContextI *)
{
    return std::vector<CuData>();
}

QObject *InfoDialogExtension::get_qobject()
{
    return this;
}

const CuContextI *InfoDialogExtension::getContextI() const {
    return d->ctx;
}


std::string InfoDialogExtension::message() const{
    return d->msg;
}

bool InfoDialogExtension::error() const {
    return d->err;
}
