#include <quapplication.h>
#include "manual_refresh.h"

// cumbia-tango
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia-tango

#include <X11/Xlib.h>
#include <QX11Info>


#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("Elettra");
    qu_app.setApplicationName("Manual Mode example");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "+390403758073");
    qu_app.setProperty("office", "$AU_OFFICE$");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    
    // instantiate CumbiaTango
    CumbiaPool *cu_p = new CumbiaPool();

    Manual_refresh *w = new Manual_refresh(cu_p, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    // exec application loop
    int ret = qu_app.exec();
    // delete resources and return
    delete w;
    delete cu_p;
    return ret;
}
