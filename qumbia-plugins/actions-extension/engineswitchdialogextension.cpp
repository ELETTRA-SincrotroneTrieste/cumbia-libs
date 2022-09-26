#include "engineswitchdialogextension.h"
#include "cuengineswitchdialog.h"

class EngineSwitchDialogExtensionPrivate {
public:
    EngineSwitchDialogExtensionPrivate(const CuContextI* ci) : ctxi(ci) {}
    const CuContextI * ctxi;
    std::string msg;
};

EngineSwitchDialogExtension::EngineSwitchDialogExtension(const CuContextI *ctx_i, QWidget* parent )
    : QObject{parent} {
    d = new EngineSwitchDialogExtensionPrivate(ctx_i);
}

EngineSwitchDialogExtension::~EngineSwitchDialogExtension() {
    printf("deleting EngineSwitchDialogExtension\n");
    delete d;
    printf("deleted\n");
}

QString EngineSwitchDialogExtension::getName() const {
    return "EngineSwitchDialogExtension";
}

CuData EngineSwitchDialogExtension::execute(const CuData &in, const CuContextI *ctxI) {
//#if defined QUMBIA_TANGO_CONTROLS_VERSION && defined CUMBIA_HTTP_VERSION
    // WA_DeleteOnClose attribute is set
    printf("\e[1;32;3mexecuting exec CuEngineSwitchDialog\e[0m\n\n\n");
    CuEngineSwitchDialog* dlg = new CuEngineSwitchDialog(nullptr);
    dlg->exec(in, ctxI);
//#else
//#endif
    return CuData();
}

std::vector<CuData> EngineSwitchDialogExtension::execute(const std::vector<CuData> &, const CuContextI *) {
    return std::vector<CuData>();
}

QObject *EngineSwitchDialogExtension::get_qobject() {
    return this;
}

const CuContextI *EngineSwitchDialogExtension::getContextI() const {
    return nullptr;
}

std::string EngineSwitchDialogExtension::message() const {
    return d->msg;
}

bool EngineSwitchDialogExtension::error() const {
    return d->msg.size() > 0;
}
