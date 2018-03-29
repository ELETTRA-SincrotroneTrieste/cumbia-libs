#include "ComplexQTangoDemo.h"
#include <TApplication>
#include <tutil.h>
#include <X11/Xlib.h>
#include <QX11Info>
#include <elettracolors.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{

    TApplication a( argc, argv );
    a.setOrganizationName("-");
    a.setApplicationName("-");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "Giacomo");
    a.setProperty("mail", "giacomo.strangolino@elettra.eu");
    a.setProperty("phone", "0403758073");
    a.setProperty("office", "T2Pt025");
    a.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
    TUtil::instance()->setLoggingTarget(argv[0]);

    ComplexQTangoDemo w;
    if(w.property("epalette").isValid())
    	a.setPalette(EPalette(w.property("epalette").toString()));
    w.show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w.winId();
    XSetCommand(disp, root_win, argv, argc);

    return a.exec();
}
