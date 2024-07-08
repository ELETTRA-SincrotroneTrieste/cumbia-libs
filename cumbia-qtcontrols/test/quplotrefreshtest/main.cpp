/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "mainwindow.h"
#include <quapplication.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cuengineaccessor.h>

int main( int argc, char* argv[] )
{
    CuControlsFactoryPool fp;
    CumbiaPool *cu_p = new CumbiaPool();
    QuApplication qu_app( argc, argv, cu_p, &fp);
    MainWindow *w = new MainWindow;
    w->resize( 800, 600 );
    w->show();

    int ret = qu_app.exec();
    // delete resources and return
    // make sure to be operating on a valid cu_p in case of engine swap at runtime
    CuEngineAccessor *c = w->findChild<CuEngineAccessor *>();
    if(c)
        cu_p = c->cu_pool();
    delete w;

    for(std::string n : cu_p->names())
        if(cu_p->get(n))
            delete cu_p->get(n);

    return ret;
}
