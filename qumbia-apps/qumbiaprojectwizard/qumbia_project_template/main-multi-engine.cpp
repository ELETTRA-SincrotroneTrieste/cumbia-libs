#include <quapplication.h>
#include "cuengineaccessor.h"
#include "$HFILE$"

// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia

#define VERSION "1.0"

int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("$ORGANIZATION_NAME$");
    qu_app.setApplicationName("$APPLICATION_NAME$");
    QString version(VERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "$AUTHOR$");
    qu_app.setProperty("mail", "$AU_EMAIL$");
    qu_app.setProperty("phone", "$AU_PHONE$");
    qu_app.setProperty("office", "$AU_OFFICE$");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    // $palette$

    CumbiaPool *cu_p = new CumbiaPool();
    $MAINCLASS$ *w = new $MAINCLASS$(cu_p, NULL);
    w->show();
    // exec application loop
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
