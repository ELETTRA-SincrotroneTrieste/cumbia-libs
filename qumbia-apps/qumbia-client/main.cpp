#include "qumbia-client.h"
#include <quapplication.h>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cucontextactionbridge.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{

    QuApplication a(argc, argv);
    CumbiaPool *cu_p = new CumbiaPool();
    QumbiaClient *w = new QumbiaClient(cu_p, NULL);
    w->show();

    int ret = a.exec();
    delete w;
    if(cu_p->get("tango"))
        delete cu_p->get("tango");
    if(cu_p->get("epics"))
        delete cu_p->get("epics");
    if(cu_p->get("random"))
        delete cu_p->get("random");

    return ret;
}
