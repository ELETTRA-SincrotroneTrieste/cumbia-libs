#include <quapplication.h>
#include "formulas.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#include <X11/Xlib.h>
#include <QX11Info>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication* qu_app = new QuApplication( argc, argv );
    qu_app->setOrganizationName("$ORGANIZATION_NAME$");
    qu_app->setApplicationName("$APPLICATION_NAME$");
    QString version(CVSVERSION);
    qu_app->setApplicationVersion(version);
    qu_app->setProperty("author", "Giacomo");
    qu_app->setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app->setProperty("phone", "0403758073");
    qu_app->setProperty("office", "T2PT025");
    qu_app->setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    

    CumbiaPool *cu_p = new CumbiaPool();
    Formulas *w = new Formulas(cu_p, NULL);
    w->show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w->winId();
    XSetCommand(disp, root_win, argv, argc);

    ret = qu_app->exec();
    delete w;
    delete qu_app;
    delete cu_p->get("tango");
    delete cu_p->get("epics");
    return ret;
}
