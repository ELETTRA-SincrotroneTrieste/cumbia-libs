#include "context.h"
#include <quapplication.h>
#include <X11/Xlib.h>
#include <QX11Info>
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication a( argc, argv );
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setApplicationName("context");
    a.setApplicationVersion("0.1");
    a.setOrganizationDomain("elettra.eu");
    a.setProperty("author", "Giacomo");
    a.setProperty("mail", "giacomo.strangolino@elettra.eu");
    a.setProperty("phone", "0403758073");
    a.setProperty("office", "giacomo");
    a.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
    CumbiaPool *cu_p = new CumbiaPool();

    Context *w = new Context(cu_p, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    ret = a.exec();
    delete w;
    delete cu_p;
    return ret;


}
