#include "multireader.h"
#include <QApplication>
#include <X11/Xlib.h>
#include <QX11Info>
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QApplication a( argc, argv );
    a.setOrganizationName("-");
    a.setApplicationName("-");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "Giacomo");
    a.setProperty("mail", "giacomo.strangolino@elettra.eu");
    a.setProperty("phone", "0403758073");
    a.setProperty("office", "giacomo");
    a.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
    CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    Multireader *w = new Multireader(cu_t, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    ret = a.exec();
    delete w;
    delete cu_t;
    return ret;


}
