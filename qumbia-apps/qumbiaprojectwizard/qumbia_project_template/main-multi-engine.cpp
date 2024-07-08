#include <quapplication.h>
#include <cuengineaccessor.h>
#include "$HFILE$"

// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia

#define VERSION "1.0"

int main(int argc, char *argv[])
{
    CumbiaPool *p = new CumbiaPool();
    CuControlsFactoryPool fpool;
    QuApplication qu_app( argc, argv, p, &fpool);
    qu_app.setOrganizationName("$ORGANIZATION_NAME$");
    qu_app.setApplicationName("$APPLICATION_NAME$");
    QString version(VERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "$AUTHOR$");
    qu_app.setProperty("mail", "$AU_EMAIL$");
    qu_app.setProperty("phone", "$AU_PHONE$");
    qu_app.setProperty("office", "$AU_OFFICE$");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    CuEngineAccessor *a = new CuEngineAccessor(&qu_app, &p, &fpool);
    $MAINCLASS$ *w = new $MAINCLASS$(nullptr);
    w->show();
    // exec application loop
    int ret = qu_app.exec();
    // delete resources and return
    // make sure to be operating on a valid p in case of engine swap at runtime
    p = a->cu_pool();
    delete w;

    for(const std::string& n : p->names())
        if(p->get(n))
            delete p->get(n);

    return ret;
}
