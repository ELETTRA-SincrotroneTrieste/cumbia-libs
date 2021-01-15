#include <quapplication.h>
#include "editargs.h"

// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia

#define VERSION "1.0"

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Usage: %s src1 src2, ... srcN\n", argv[0]);
        printf(" e.g.: %s test/device/1->DevVarDoubleArray(0,0,0,1) test/device/2->DevVarDoubleArray(0,1,0,0,0,0)\n\n", argv[1]);
        return 0;
    }

    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("Elettra");
    qu_app.setApplicationName("Editargs");
    QString version(VERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2PT025");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    CumbiaPool *cu_p = new CumbiaPool();
    Editargs *w = new Editargs(cu_p, NULL);
    w->show();
    // exec application loop
    int ret = qu_app.exec();
    // delete resources and return
    delete w;

    for(std::string n : cu_p->names())
        if(cu_p->get(n))
            delete cu_p->get(n);

    return ret;
}
