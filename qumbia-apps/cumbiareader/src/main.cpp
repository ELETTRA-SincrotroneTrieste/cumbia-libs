#include <quapplication.h>
#include "cumbiareader.h"
#include <X11/Xlib.h>
#include <QX11Info>

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    int ret;
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("eu.elettra");
    qu_app.setApplicationName("cumbiareader");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "-");
    qu_app.setProperty("office", "-");
    qu_app.setProperty("hwReferent", "-"); /* name of the referent that provides the device server */

    CumbiaPool *cu_p = new CumbiaPool();
    Cumbiareader *w = new Cumbiareader(cu_p, NULL);
    if(!w->usage_only())
        ret = qu_app.exec();
    delete w;
    if(cu_p->get("tango"))
        delete cu_p->get("tango");
    if(cu_p->get("epics"))
        delete cu_p->get("epics");
    if(cu_p->get("random"))
        delete cu_p->get("random");
    return ret;
}
