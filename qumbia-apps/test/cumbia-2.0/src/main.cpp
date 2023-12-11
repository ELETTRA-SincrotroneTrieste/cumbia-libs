#include <quapplication.h>
#include <cuengineaccessor.h>
#include "cumbia-2.0.h"

// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia

#define VERSION "1.0"

int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("Elettra");
    qu_app.setApplicationName("Cumbia2.0");
    QString version(VERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2PT025");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    // $palette$

    CumbiaPool *cu_p = new CumbiaPool();
    Cumbia2 *w = new Cumbia2(cu_p, NULL);
//    w->show();
    // exec application loop
    int ret = 0; // qu_app.exec();
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
