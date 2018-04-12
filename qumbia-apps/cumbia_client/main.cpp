#include "cumbia_client.h"
#include <quapplication.h>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cucontextactionbridge.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{

    QuApplication a(argc, argv);
    CumbiaPool *cu_p = new CumbiaPool();
    CumbiaClient *w = new CumbiaClient(cu_p, NULL);
    w->show();

    int ret = a.exec();
    delete w;
    delete cu_p->get("tango");
#ifdef QUMBIA_EPICS_CONTROLS
    delete cu_p->get("epics");
#endif
    return ret;
}
