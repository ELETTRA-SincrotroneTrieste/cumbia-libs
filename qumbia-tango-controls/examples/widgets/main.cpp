#include "cutcwidgets.h"
#include <QApplication>
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    CuTCWidgets *w = new CuTCWidgets(cu_t);
    w->show();

    int ret = a.exec();
    delete w;
    delete cu_t;
    return ret;
}
