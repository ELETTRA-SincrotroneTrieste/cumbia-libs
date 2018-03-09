#include "label.h"
#include <QApplication>
#include <QMessageBox>
#include <cumbiaepics.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(qApp->arguments().count() < 2)
    {
        QMessageBox::information(0, "Usage", argv[0] + QString(" test/device/name/attribute|command"));
        exit(EXIT_FAILURE);
    }
    else
    {
        CumbiaEpics *cu_epi = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        WidgetWithLabel *w = new WidgetWithLabel(cu_epi);
        w->show();
        // The following calls in this order
        int ret = a.exec();  // 1
        delete w;            // 2 first delete the widget
        delete cu_epi;       // 3 last delete the cumbia instance
        return ret;
    }

}
