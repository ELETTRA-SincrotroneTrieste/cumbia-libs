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
    CumbiaPool *cu_p = new CumbiaPool();

    CreateDelete *w = new CreateDelete(cu_p, NULL);

    w->show();

    int ret = a.exec();
    delete w;
    delete cu_p->get("tango");
    delete cu_p->get("epics");
    return ret;
}
