#include <quapplication.h>
#include <cuengineaccessor.h>
#include "cuengineaccessor_test.h"

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
    qu_app.setApplicationName("CuengineaccessorTest");
    QString version(VERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2PT025");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    CuengineaccessorTest *w = new CuengineaccessorTest(nullptr);
    w->show();
    // exec application loop
    int ret = qu_app.exec();
    CumbiaPool *cu_pool = nullptr;
    CuEngineAccessor *a = w->findChild<CuEngineAccessor *>();
    if(a)
        cu_pool = a->cu_pool();
    delete w;
    // delete resources and return
    if(cu_pool) {
        for(const std::string& n : cu_pool->names()) {
            printf("cumbia type '%s'\n", n.c_str());
            if(cu_pool->get(n)) {
                printf("- deleting %p (%s)\n", cu_pool->get(n), n.c_str());
                delete cu_pool->get(n);
            }
        }
    }
    // make sure to be operating on a valid cu_p in case of engine swap at runtime
    return ret;
}
