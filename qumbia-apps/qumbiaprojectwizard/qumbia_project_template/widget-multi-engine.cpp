#include "$HFILE$"
#include "ui_$HFILE$"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia

$MAINCLASS$::$MAINCLASS$(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::$UIFORMCLASS$;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);
    // provide access to the engine in case of runtime swap
    new CuEngineAccessor(this, &cu_pool, &m_ctrl_factory_pool);

    // mloader.modules() to get the list of loaded modules
    // cumbia
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
