#include "cuengineaccessor_test.h"
#include "ui_cuengineaccessor_test.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
#include <quapplication.h>
// cumbia

CuengineaccessorTest::CuengineaccessorTest(QWidget *parent) :
    QWidget(parent) {
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    CuModuleLoader mloader(a->cumbiaPool(), a->fpool(), &m_log_impl);
    ui = new Ui::Cuengineaccessor;
    ui->setupUi(this);
}

CuengineaccessorTest::~CuengineaccessorTest() {
    delete ui;
}
