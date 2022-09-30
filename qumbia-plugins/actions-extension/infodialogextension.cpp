#include "infodialogextension.h"
#include "cuinfodialog.h"

#include <cucontext.h>

class InfoDialogExtensionPrivate {
public:
    InfoDialogExtensionPrivate(const CuContextI *cctx) : ctx(cctx), dialog(nullptr) {}
    std::string msg;
    bool err;
    const CuContextI *ctx;
    CuInfoDialog* dialog;
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
    if(!d->dialog) {
        printf("InfoDialogExtension %p creating dialog\n", this);
        d->dialog = new CuInfoDialog(nullptr);
        connect(d->dialog, SIGNAL(destroyed(QObject *)), this, SLOT(m_dialog_destroyed(QObject *)));
        d->dialog->exec(in, ctxI);
    }
    else {
        printf("\e[1;33mInfoDialogExtension: raising an existing dialog\e[0m\n");
        d->dialog->raise();
    }
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

void InfoDialogExtension::m_dialog_destroyed(QObject *o) {
    d->dialog = nullptr;
}
