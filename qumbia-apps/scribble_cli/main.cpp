#include "generic_client.h"
#include <QApplication>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    CumbiaPool *cu_p = new CumbiaPool();
    GenericClient *w = new GenericClient(cu_p, NULL);
    w->show();

    int ret = a.exec();
    delete w;
    delete cu_p->get("tango");
    delete cu_p->get("epics");
    return ret;
}
