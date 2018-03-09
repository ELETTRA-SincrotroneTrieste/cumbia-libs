#include "$HFILE$"
#include <X11/Xlib.h>
#include <QX11Info>

// cumbia include files
#include <quapplication.h>
#include <cumbiatango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// end cumbia include files

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication a( argc, argv );
    a.setOrganizationName("$ORGANIZATION_NAME$");
    a.setApplicationName("$APPLICATION_NAME$");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "$AUTHOR$");
    a.setProperty("mail", "$AU_EMAIL$");
    a.setProperty("phone", "$AU_PHONE$");
    a.setProperty("office", "$AU_OFFICE$");
    a.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    
    // instantiate CumbiaTango
    CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    $MAINCLASS$ *w = new $MAINCLASS$(cu_t, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    // exec application loop
    ret = a.exec();
    // delete resources and return
    delete w;
    delete cu_t;
    return ret;


}
