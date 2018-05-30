#include "bpmqumbia.h"
// #include <TApplication>
// cumbia-tango
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia-tango

#include <quapplication.h>//	TApplication is mapped into QuApplication. DBus connection is provided by the cumbia dbus plugin	//	 ## added by qumbiaprojectwizard
// #include <tutil.h>
// no cumbia include replacement found for tutil.h
#include <X11/Xlib.h>
#include <QX11Info>
// #include <elettracolors.h>
// no cumbia include replacement found for elettracolors.h

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{

//     TApplication qu_app( argc, argv );
    QuApplication qu_app( argc, argv );//	TApplication is mapped into QuApplication. DBus connection is provided by cumbia dbus plugin	//	 ## added by qumbiaprojectwizard
    qu_app.setOrganizationName("-");
    qu_app.setApplicationName("-");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2Pt025");
    qu_app.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
//     TUtil::instance()->setLoggingTarget(argv[0]);
//	QTango TUtil:: class is not present in cumbia	//	 ## added by qumbiaprojectwizard

    CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());



    Bpmqtango *w = new Bpmqtango(cu_t, 0);
//     if(w->property("epalette").isValid())
//	EPalette is not currently available in cumbia	//	 ## added by qumbiaprojectwizard
//     	qu_app.setPalette(EPalette(w->property("epalette").toString()));
//	EPalette is not currently available in cumbia	//	 ## added by qumbiaprojectwizard
    w->show();

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

