#include "cuepwidgets.h"
#include <QApplication>
#include <cumbiaepics.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    CumbiaEpics *cu_t = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    CuEpWidgets *w = new CuEpWidgets(cu_t);
    w->show();

    int ret = a.exec();
    delete w;
    delete cu_t;
    return ret;
}
