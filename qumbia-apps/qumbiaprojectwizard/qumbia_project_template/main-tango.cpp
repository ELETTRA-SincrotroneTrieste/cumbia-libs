#include "$HFILE$"
#include <X11/Xlib.h>
#include <QX11Info>

#include <quapplication.h>

// cumbia-tango
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia-tango

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("$ORGANIZATION_NAME$");
    qu_app.setApplicationName("$APPLICATION_NAME$");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "$AUTHOR$");
    qu_app.setProperty("mail", "$AU_EMAIL$");
    qu_app.setProperty("phone", "$AU_PHONE$");
    qu_app.setProperty("office", "$AU_OFFICE$");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    
    // instantiate CumbiaTango
    CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    $MAINCLASS$ *w = new $MAINCLASS$(cu_t, NULL);
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
