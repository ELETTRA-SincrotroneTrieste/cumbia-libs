#include "$HFILE$"
#include <QApplication>
#include <X11/Xlib.h>
#include <QX11Info>

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QApplication a( argc, argv );
    a.setOrganizationName("$ORGANIZATION_NAME$");
    a.setApplicationName("$APPLICATION_NAME$");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "$AUTHOR$");
    a.setProperty("mail", "$AU_EMAIL$");
    a.setProperty("phone", "$AU_PHONE$");
    a.setProperty("office", "$AU_OFFICE$");
    a.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    

    CumbiaPool *cu_p = new CumbiaPool();
    $MAINCLASS$ *w = new $MAINCLASS$(cu_p, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    ret = a.exec();
    delete w;
    delete cu_p->get("tango");
    delete cu_p->get("epics");
    return ret;
}
