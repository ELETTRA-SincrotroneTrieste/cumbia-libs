#include "$HFILE$"
#include "ui_$HFILE$"

// cumbia
#include <cumbiapool.h>
#include <cumacros.h>
#include <quapplication.h>
#include <quapps.h>
// cumbia

$MAINCLASS$::$MAINCLASS$(QWidget *parent) : QWidget(parent) {
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    CuModuleLoader mloader(a->cumbiaPool(), a->fpool(), &m_log_impl);
    ui = new Ui::$UIFORMCLASS$;
    ui->setupUi(this);
    // mloader.modules() to get the list of loaded modules
}

$MAINCLASS$::~$MAINCLASS$() {
    delete ui;
}
