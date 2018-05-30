#include <quapplication.h>
#include "cumbiaplots.h"
#include <X11/Xlib.h>
#include <QX11Info>


// cumbia-tango
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia-tango

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("-");
    qu_app.setApplicationName("-");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "-");
    qu_app.setProperty("phone", "-");
    qu_app.setProperty("office", "-");
    qu_app.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
    // instantiate CumbiaTango
    CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    Cumbiaplots *w = new Cumbiaplots(cu_t, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    // exec application loop
    int ret = qu_app.exec();
    // delete resources and return
    delete w;
    delete cu_t;
    return ret;
}
