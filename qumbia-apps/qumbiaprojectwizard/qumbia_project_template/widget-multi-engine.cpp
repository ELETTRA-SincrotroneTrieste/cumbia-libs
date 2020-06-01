#include "$HFILE$"
#include "ui_$HFILE$"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <quapps.h>

$MAINCLASS$::$MAINCLASS$(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::$UIFORMCLASS$)
{
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // mloader.modules() to get the list of loaded modules
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
