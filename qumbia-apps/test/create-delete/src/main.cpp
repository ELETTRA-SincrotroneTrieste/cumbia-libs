#include "create-delete.h"
#include <QApplication>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <X11/Xlib.h>

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );
    a.setApplicationName("CreateDelete");
    CumbiaPool *cu_poo = new CumbiaPool();

    CreateDelete *w = new CreateDelete(cu_poo, NULL);

    w->show();

    int ret = a.exec();
    delete w;
    delete cu_poo->get("tango");
#ifdef QUMBIA_EPICS_CONTROLS
    delete cu_poo->get("epics");
#endif
    delete cu_poo;
    return ret;
}
