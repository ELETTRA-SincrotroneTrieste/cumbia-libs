#include <QApplication>
#include "qtangoplots.h"
#include <X11/Xlib.h>
#include <QX11Info>


#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    QApplication qu_app( argc, argv );
    qu_app.setOrganizationName("-");
    qu_app.setApplicationName("-");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "-");
    qu_app.setProperty("phone", "-");
    qu_app.setProperty("office", "-");
    qu_app.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */
    
    QTangoPlots *w = new QTangoPlots(NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    // exec application loop
    int ret = qu_app.exec();
    // delete resources and return
    delete w;
    return ret;
}
